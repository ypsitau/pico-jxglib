#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "tusb.h"
#include <string.h>
#include "fx2lafw.h"

// fx2lafw Protocol Commands
#define CMD_GET_FW_VERSION      0xB0
#define CMD_START               0xB1
#define CMD_GET_REVID           0xB2
#define CMD_SET_COUPLING        0xB4
#define CMD_SET_SAMPLERATE      0xB5
#define CMD_SET_NUMSAMPLES      0xB6

// fx2lafw Protocol Values
#define FX2LAFW_COUPLING_AC     0x00
#define FX2LAFW_COUPLING_DC     0x01

// fx2lafw compatible VID/PID
#define FX2LAFW_VID             0x1D50
#define FX2LAFW_PID             0x608C

// Configuration
#define LOGIC_ANALYZER_CHANNELS 8
#define SAMPLE_BUFFER_SIZE      1024
#define BULK_EP_IN              0x81
#define BULK_EP_OUT             0x02

// String descriptor buffer
static uint16_t desc_str_buffer[32];

// Global variables
static bool capturing = false;
static uint32_t sample_rate = 1000000; // 1MHz default
static uint32_t num_samples = 1024;    // Number of samples to capture
static uint8_t sample_buffer[SAMPLE_BUFFER_SIZE];
static uint16_t buffer_pos = 0;
static uint32_t samples_captured = 0;
static absolute_time_t next_sample_time;

//-----------------------------------------------------------------------------
// USB Descriptors
//-----------------------------------------------------------------------------
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0xFF,  // Vendor-specific class
    .bDeviceSubClass    = 0xFF,  // Vendor-specific subclass
    .bDeviceProtocol    = 0xFF,  // Vendor-specific protocol
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = FX2LAFW_VID,
    .idProduct          = FX2LAFW_PID,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

enum {
    ITF_NUM_VENDOR = 0,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)

uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, 0x00, 100),
    TUD_VENDOR_DESCRIPTOR(ITF_NUM_VENDOR, 0, BULK_EP_OUT, BULK_EP_IN, 64)
};

char const* string_desc_arr[] = {
    (const char[]) { 0x09, 0x04 }, // 0: supported language is English (0x0409)
    "sigrok",                      // 1: Manufacturer  
    "fx2lafw",                     // 2: Product
    "fx2lafw",                     // 3: Serial Number (keep it simple)
};

//-----------------------------------------------------------------------------
// USB Callbacks
//-----------------------------------------------------------------------------
uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return desc_configuration;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;
    
    uint8_t chr_count;
    if (index == 0) {
        memcpy(&desc_str_buffer[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if (!(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))) return NULL;
        
        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;
        
        for (uint8_t i = 0; i < chr_count; i++) {
            desc_str_buffer[1+i] = str[i];
        }
    }
    
    desc_str_buffer[0] = (TUSB_DESC_STRING << 8) | (2*chr_count + 2);
    return desc_str_buffer;
}

//-----------------------------------------------------------------------------
// Vendor Class Implementation
//-----------------------------------------------------------------------------

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
  // nothing to with DATA & ACK stage
  if (stage != CONTROL_STAGE_SETUP) return true;

  switch (request->bmRequestType_bit.type)
  {
    case TUSB_REQ_TYPE_VENDOR:
      switch (request->bRequest)
      {
        case CMD_GET_FW_VERSION:
          // PulseView expects exactly this string format
          return tud_control_xfer(rhport, request, (void*) "fx2lafw 0.1.7", 13);

        case CMD_START:
          capturing = true;
          buffer_pos = 0;
          samples_captured = 0;
          next_sample_time = get_absolute_time();
          printf("Starting capture: %lu samples at %lu Hz\n", num_samples, sample_rate);
          return tud_control_status(rhport, request);

        case CMD_GET_REVID:
        {
          // Return revision ID that PulseView expects
          uint8_t revid = 0x01;
          return tud_control_xfer(rhport, request, &revid, 1);
        }

        case CMD_SET_COUPLING:
          // Just acknowledge - coupling doesn't apply to digital logic analyzer
          printf("Set coupling: %d\n", request->wValue);
          return tud_control_status(rhport, request);

        case CMD_SET_SAMPLERATE:
        {
          // Sample rate calculation for fx2lafw protocol
          // The rate is calculated as: 48MHz / (2^divider)
          uint8_t divider = request->wValue;
          if (divider < 8) {  // Reasonable range
            sample_rate = 48000000 >> divider;
            printf("Sample rate set to: %lu Hz (divider: %d)\n", sample_rate, divider);
          }
          return tud_control_status(rhport, request);
        }

        case CMD_SET_NUMSAMPLES:
        {
          // Number of samples from wValue and wIndex
          num_samples = (request->wIndex << 16) | request->wValue;
          printf("Number of samples set to: %lu\n", num_samples);
          return tud_control_status(rhport, request);
        }

        default: break;
      }
    break;

    default: break;
  }

  // stall unknown request
  return false;
}

// Invoked when DATA Stage of VENDOR's request is complete
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const * request)
{
  (void) rhport;
  (void) request;

  // nothing to do
  return true;
}

void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize)
{
  (void) itf;
  (void) buffer;
  (void) bufsize;
  
  // Handle any incoming data if needed
}

//-----------------------------------------------------------------------------
// Logic Analyzer Functions
//-----------------------------------------------------------------------------
void init_gpio_pins(void) {
    // Initialize GPIO pins for logic analyzer input
    for (int i = 0; i < LOGIC_ANALYZER_CHANNELS; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_IN);
        gpio_pull_up(i);
    }
}

uint8_t sample_gpio(void) {
    uint8_t sample = 0;
    for (int i = 0; i < LOGIC_ANALYZER_CHANNELS; i++) {
        if (gpio_get(i)) {
            sample |= (1 << i);
        }
    }
    return sample;
}

void process_sampling(void) {
    if (!capturing || !tud_mounted()) return;
    
    // Check if we've captured enough samples
    if (samples_captured >= num_samples) {
        capturing = false;
        printf("Capture complete: %lu samples\n", samples_captured);
        return;
    }
    
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(next_sample_time, now) >= 0) {
        // Time to take a sample
        sample_buffer[buffer_pos++] = sample_gpio();
        samples_captured++;
        
        // Calculate next sample time
        uint64_t interval_us = 1000000 / sample_rate;
        next_sample_time = delayed_by_us(next_sample_time, interval_us);
        
        // Send buffer when full or when capture is complete
        if (buffer_pos >= SAMPLE_BUFFER_SIZE || samples_captured >= num_samples) {
            // For now, just print the data for debugging
            // In a full implementation, you would send via bulk endpoint
            printf("Buffer: %d samples\n", buffer_pos);
            buffer_pos = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void) {
    stdio_init_all();
    
    // Initialize GPIO pins for logic analyzer
    init_gpio_pins();
    
    // Initialize USB
    tusb_init();
    
    printf("fx2lafw Logic Analyzer started\n");
    
    while (1) {
        tud_task(); // TinyUSB device task
        process_sampling();
        
        sleep_us(10); // Small delay to prevent overwhelming the CPU
    }
}
