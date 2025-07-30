#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "tusb.h"
#include <string.h>
#include <stdlib.h>
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
#define LOGIC_ANALYZER_START_PIN 2  // Start from GPIO2 instead of GPIO0
#define SAMPLE_BUFFER_SIZE      1024
#define BULK_EP_IN              0x81
#define BULK_EP_OUT             0x02

// String descriptor buffer
static uint16_t desc_str_buffer[32];

// Global variables
static bool capturing = false;
static bool capture_complete = false;
static uint32_t sample_rate = 1000000; // 1MHz default
static uint32_t num_samples = 1024;    // Number of samples to capture
static uint8_t sample_buffer[SAMPLE_BUFFER_SIZE];
static uint16_t buffer_pos = 0;
static uint32_t samples_captured = 0;
static absolute_time_t next_sample_time;

// Bulk transfer variables
static uint8_t* bulk_buffer = NULL;
static uint32_t bulk_buffer_size = 0;
static uint32_t bulk_bytes_sent = 0;
static bool bulk_transfer_active = false;

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
    printf("USB: Device descriptor requested\n");
    return (uint8_t const*)&desc_device;
}

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    printf("USB: Configuration descriptor requested (index=%d)\n", index);
    (void)index;
    return desc_configuration;
}

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    printf("USB: String descriptor requested (index=%d, langid=0x%04x)\n", index, langid);
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

  printf("Vendor control request: type=0x%02x, request=0x%02x, wValue=0x%04x, wIndex=0x%04x\n",
         request->bmRequestType, request->bRequest, request->wValue, request->wIndex);

  switch (request->bmRequestType_bit.type)
  {
    case TUSB_REQ_TYPE_VENDOR:
      switch (request->bRequest)
      {
        case CMD_GET_FW_VERSION:
          // PulseView expects exactly this string format
          printf("  -> CMD_GET_FW_VERSION\n");
          return tud_control_xfer(rhport, request, (void*) "fx2lafw 0.1.7", 13);

        case CMD_START:
          // Free previous buffer if exists
          if (bulk_buffer) {
            free(bulk_buffer);
            bulk_buffer = NULL;
          }
          
          capturing = true;
          capture_complete = false;
          buffer_pos = 0;
          samples_captured = 0;
          bulk_bytes_sent = 0;
          bulk_transfer_active = false;
          next_sample_time = get_absolute_time();
          
          // Allocate buffer for captured data
          bulk_buffer_size = num_samples;
          bulk_buffer = (uint8_t*)malloc(bulk_buffer_size);
          
          if (bulk_buffer) {
            printf("  -> CMD_START: Starting capture: %lu samples at %lu Hz\n", num_samples, sample_rate);
            printf("  -> Allocated buffer: %lu bytes for bulk transfer\n", bulk_buffer_size);
            // Clear buffer
            memset(bulk_buffer, 0, bulk_buffer_size);
          } else {
            printf("  -> CMD_START: Failed to allocate buffer!\n");
            capturing = false;
          }
          return tud_control_status(rhport, request);

        case CMD_GET_REVID:
        {
          // Return revision ID that PulseView expects
          uint8_t revid = 0x01;
          printf("  -> CMD_GET_REVID: returning 0x%02x\n", revid);
          return tud_control_xfer(rhport, request, &revid, 1);
        }

        case CMD_SET_COUPLING:
          // Just acknowledge - coupling doesn't apply to digital logic analyzer
          printf("  -> CMD_SET_COUPLING: %d\n", request->wValue);
          return tud_control_status(rhport, request);

        case CMD_SET_SAMPLERATE:
        {
          // Sample rate calculation for fx2lafw protocol
          // The rate is calculated as: 48MHz / (2^divider)
          uint8_t divider = request->wValue;
          if (divider < 8) {  // Reasonable range
            sample_rate = 48000000 >> divider;
            printf("  -> CMD_SET_SAMPLERATE: divider=%d, rate=%lu Hz\n", divider, sample_rate);
          } else {
            printf("  -> CMD_SET_SAMPLERATE: invalid divider %d\n", divider);
          }
          return tud_control_status(rhport, request);
        }

        case CMD_SET_NUMSAMPLES:
        {
          // Number of samples from wValue and wIndex
          num_samples = (request->wIndex << 16) | request->wValue;
          printf("  -> CMD_SET_NUMSAMPLES: %lu samples\n", num_samples);
          return tud_control_status(rhport, request);
        }

        default: 
          printf("  -> Unknown vendor request: 0x%02x\n", request->bRequest);
          break;
      }
    break;

    default: 
      printf("  -> Non-vendor request type: 0x%02x\n", request->bmRequestType_bit.type);
      break;
  }

  // stall unknown request
  printf("  -> Stalling unknown request\n");
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
  printf("Vendor RX callback: received %d bytes\n", bufsize);
  
  // If this is a bulk data request and we have captured data
  if (capture_complete && bulk_buffer && bufsize == 0) {
    printf("Host requesting bulk data, have %lu bytes ready\n", bulk_buffer_size);
  }
  
  // Echo back any received data for testing
  if (bufsize > 0) {
    printf("RX data: ");
    for (int i = 0; i < bufsize && i < 8; i++) {
      printf("0x%02x ", buffer[i]);
    }
    printf("\n");
  }
}

// Vendor TX complete callback
void tud_vendor_tx_cb(uint8_t itf, uint32_t sent_bytes)
{
  (void) itf;
  printf("Vendor TX complete: %lu bytes sent (total: %lu/%lu)\n", 
         sent_bytes, bulk_bytes_sent, bulk_buffer_size);
  
  // Continue sending if there's more data and transfer is active
  if (bulk_transfer_active && bulk_buffer && bulk_bytes_sent < bulk_buffer_size) {
    uint32_t available = tud_vendor_write_available();
    uint32_t remaining = bulk_buffer_size - bulk_bytes_sent;
    uint32_t to_send = (remaining > available) ? available : remaining;
    if (to_send > 64) to_send = 64; // Packet size limit
    
    if (to_send > 0) {
      uint32_t sent = tud_vendor_write(bulk_buffer + bulk_bytes_sent, to_send);
      if (sent > 0) {
        bulk_bytes_sent += sent;
        printf("TX callback: sent %lu more bytes, total: %lu/%lu\n", 
               sent, bulk_bytes_sent, bulk_buffer_size);
        tud_vendor_write_flush();
        
        if (bulk_bytes_sent >= bulk_buffer_size) {
          printf("*** Bulk transfer complete via TX callback ***\n");
          bulk_transfer_active = false;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Logic Analyzer Functions
//-----------------------------------------------------------------------------
void init_gpio_pins(void) {
    // Initialize GPIO pins for logic analyzer input
    // Start from GPIO2 to avoid conflict with UART (GPIO0/1)
    printf("Setting up %d GPIO pins starting from GPIO%d\n", LOGIC_ANALYZER_CHANNELS, LOGIC_ANALYZER_START_PIN);
    for (int i = 0; i < LOGIC_ANALYZER_CHANNELS; i++) {
        gpio_init(LOGIC_ANALYZER_START_PIN + i);
        gpio_set_dir(LOGIC_ANALYZER_START_PIN + i, GPIO_IN);
        gpio_pull_up(LOGIC_ANALYZER_START_PIN + i);
        printf("  CH%d: GPIO%d configured\n", i, LOGIC_ANALYZER_START_PIN + i);
    }
    printf("All GPIO pins configured with pull-up resistors\n");
}

uint8_t sample_gpio(void) {
    uint8_t sample = 0;
    for (int i = 0; i < LOGIC_ANALYZER_CHANNELS; i++) {
        if (gpio_get(LOGIC_ANALYZER_START_PIN + i)) {
            sample |= (1 << i);
        }
    }
    return sample;
}

void process_sampling(void) {
    static uint32_t last_debug_time = 0;
    static uint32_t debug_interval = 2000; // 2 seconds
    
    if (!tud_mounted()) return;
    
    // If capture is complete, handle bulk transfer properly
    if (capture_complete && bulk_buffer && bulk_buffer_size > 0) {
        if (!bulk_transfer_active) {
            printf("*** Starting bulk transfer: %lu bytes ***\n", bulk_buffer_size);
            bulk_transfer_active = true;
            bulk_bytes_sent = 0;
            
            // Start the first transfer
            uint32_t available = tud_vendor_write_available();
            uint32_t to_send = (bulk_buffer_size > available) ? available : bulk_buffer_size;
            if (to_send > 64) to_send = 64; // Packet size limit
            
            if (to_send > 0) {
                uint32_t sent = tud_vendor_write(bulk_buffer, to_send);
                if (sent > 0) {
                    bulk_bytes_sent = sent;
                    printf("Initial bulk transfer: %lu bytes sent\n", sent);
                    tud_vendor_write_flush();
                } else {
                    printf("Initial bulk transfer failed\n");
                }
            } else {
                printf("No TX buffer available for initial transfer\n");
            }
        }
        
        // Monitor transfer progress (TX callback handles continuation)
        static uint32_t last_progress_msg = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (bulk_transfer_active && (now - last_progress_msg > 1000)) {
            printf("Bulk transfer status: %lu/%lu bytes (%.1f%%), available=%lu\n", 
                   bulk_bytes_sent, bulk_buffer_size,
                   (float)bulk_bytes_sent / bulk_buffer_size * 100.0f,
                   tud_vendor_write_available());
            last_progress_msg = now;
        }
        
        return;
    }
    
    if (!capturing) return;
    
    // Check if we've captured enough samples
    if (samples_captured >= num_samples) {
        capturing = false;
        capture_complete = true;
        printf("*** Capture complete: %lu samples collected ***\n", samples_captured);
        return;
    }
    
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(next_sample_time, now) >= 0) {
        // Time to take a sample
        uint8_t sample = sample_gpio();
        
        // Store directly to bulk buffer
        if (bulk_buffer && samples_captured < bulk_buffer_size) {
            bulk_buffer[samples_captured] = sample;
        }
        
        samples_captured++;
        
        // Calculate next sample time
        uint64_t interval_us = 1000000 / sample_rate;
        next_sample_time = delayed_by_us(next_sample_time, interval_us);
        
        // Debug output for first few samples
        if (samples_captured <= 5) {
            printf("Sample %lu: 0x%02x\n", samples_captured, sample);
        }
    }
    
    // Periodic debug output during capture
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (capturing && (current_time - last_debug_time > debug_interval)) {
        printf("Sampling progress: %lu/%lu samples (%.1f%%)\n", 
               samples_captured, num_samples, 
               (float)samples_captured / num_samples * 100.0f);
        last_debug_time = current_time;
    }
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void) {
    stdio_init_all();
    printf("=== fx2lafw Logic Analyzer Starting ===\n");

    // Initialize GPIO pins for logic analyzer
    printf("Initializing GPIO pins %d-%d...\n", LOGIC_ANALYZER_START_PIN, LOGIC_ANALYZER_START_PIN + LOGIC_ANALYZER_CHANNELS - 1);
    init_gpio_pins();
    printf("GPIO pins initialized successfully\n");
    
    // Initialize USB
    printf("Initializing TinyUSB...\n");
    tusb_init();
    printf("TinyUSB initialization complete\n");
    
    printf("fx2lafw Logic Analyzer started\n");
    printf("Waiting for USB enumeration...\n");
    
    uint32_t last_status_time = 0;
    bool last_mounted_state = false;
    
    while (1) {
        tud_task(); // TinyUSB device task
        
        // Print status every 5 seconds
        uint32_t current_time = to_ms_since_boot(get_absolute_time());
        if (current_time - last_status_time > 5000) {
            bool mounted = tud_mounted();
            if (mounted != last_mounted_state) {
                if (mounted) {
                    printf("USB device mounted and ready\n");
                } else {
                    printf("USB device not mounted\n");
                }
                last_mounted_state = mounted;
            }
            
            if (capturing) {
                printf("Capturing... samples: %lu/%lu, rate: %lu Hz\n", 
                       samples_captured, num_samples, sample_rate);
            }
            
            last_status_time = current_time;
        }
        
        process_sampling();
        
        sleep_us(10); // Small delay to prevent overwhelming the CPU
    }
}
