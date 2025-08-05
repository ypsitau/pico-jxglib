/*
 * fx2lafw compatible logic analyzer implementation for Raspberry Pi Pico
 * Minimal implementation for USB communication reference with PulseView
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "tusb.h"

// fx2lafw compatible definitions
#define FX2LAFW_VERSION_MAJOR   1
#define FX2LAFW_VERSION_MINOR   0

// USB vendor commands (compatible with fx2lafw)
#define CMD_GET_FW_VERSION      0xb0
#define CMD_START               0xb1
#define CMD_GET_REVID_VERSION   0xb2

// Command flags (fx2lafw compatible)
#define CMD_START_FLAGS_WIDE_POS        5
#define CMD_START_FLAGS_CLK_SRC_POS     6
#define CMD_START_FLAGS_SAMPLE_8BIT     (0 << CMD_START_FLAGS_WIDE_POS)
#define CMD_START_FLAGS_SAMPLE_16BIT    (1 << CMD_START_FLAGS_WIDE_POS)
#define CMD_START_FLAGS_CLK_30MHZ       (0 << CMD_START_FLAGS_CLK_SRC_POS)
#define CMD_START_FLAGS_CLK_48MHZ       (1 << CMD_START_FLAGS_CLK_SRC_POS)

// GPIO pins for sampling (avoiding GPIO 0,1 used by UART)
#define SAMPLE_PIN_BASE         2
#define SAMPLE_PIN_COUNT        8

// Buffer configuration
#define SAMPLE_BUFFER_SIZE      512     // Minimal buffer
#define USB_EP_SIZE             64      // USB endpoint size

// Data structures (fx2lafw compatible)
struct version_info {
    uint8_t major;
    uint8_t minor;
} __attribute__((packed));

struct cmd_start_acquisition {
    uint8_t flags;
    uint8_t sample_delay_h;
    uint8_t sample_delay_l;
} __attribute__((packed));

// Global variables
static volatile bool sampling_active = false;
static volatile bool sample_16bit = false;
static uint8_t sample_buffer[SAMPLE_BUFFER_SIZE];
static volatile uint32_t buffer_pos = 0;
static uint32_t sample_count = 0;
static struct cmd_start_acquisition pending_cmd; // Store command data

// Function prototypes
void setup_gpio_sampling(void);
void generate_test_data(void);

// Setup GPIO pins for sampling
void setup_gpio_sampling(void) {
    for (int i = 0; i < SAMPLE_PIN_COUNT; i++) {
        gpio_init(SAMPLE_PIN_BASE + i);
        gpio_set_dir(SAMPLE_PIN_BASE + i, GPIO_IN);
        gpio_disable_pulls(SAMPLE_PIN_BASE + i);
    }
    printf("GPIO pins %d-%d configured for sampling\n", 
           SAMPLE_PIN_BASE, SAMPLE_PIN_BASE + SAMPLE_PIN_COUNT - 1);
}

// Generate test data for demonstration
void generate_test_data(void) {
    if (!sampling_active) return;
    
    // Generate simple test pattern (square wave)
    static uint32_t pattern_counter = 0;
    uint8_t test_value = (pattern_counter++ / 10) % 2 ? 0xFF : 0x00;
    
    if (sample_16bit) {
        // 16-bit mode: store 2 bytes per sample
        if (buffer_pos + 1 < SAMPLE_BUFFER_SIZE) {
            sample_buffer[buffer_pos++] = test_value;
            sample_buffer[buffer_pos++] = test_value >> 4; // Different pattern for high byte
        }
    } else {
        // 8-bit mode: store 1 byte per sample
        if (buffer_pos < SAMPLE_BUFFER_SIZE) {
            sample_buffer[buffer_pos++] = test_value;
        }
    }
    
    sample_count++;
    
    // Stop after collecting some samples
    if (sample_count > 100) {
        sampling_active = false;
        printf("Test sampling completed, %d samples collected\n", sample_count);
    }
}

// Handle vendor control requests (fx2lafw compatible)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    if (stage != CONTROL_STAGE_SETUP) return true;
    
    printf("USB Control Request: bRequest=0x%02X, wValue=0x%04X, wLength=%d\n", 
           request->bRequest, request->wValue, request->wLength);
    
    switch (request->bRequest) {
        case CMD_GET_FW_VERSION: {
            struct version_info version = {
                .major = FX2LAFW_VERSION_MAJOR,
                .minor = FX2LAFW_VERSION_MINOR
            };
            printf("Sending firmware version: %d.%d\n", version.major, version.minor);
            return tud_control_xfer(rhport, request, &version, sizeof(version));
        }
        
        case CMD_START: {
            printf("Received START command, wLength=%d\n", request->wLength);
            if (request->wLength == sizeof(struct cmd_start_acquisition)) {
                // Read the command data directly
                return tud_control_xfer(rhport, request, &pending_cmd, sizeof(pending_cmd));
            }
            return false;
        }
        
        case CMD_GET_REVID_VERSION: {
            uint16_t revid = 0x0100; // Revision ID
            printf("Sending revision ID: 0x%04X\n", revid);
            return tud_control_xfer(rhport, request, &revid, sizeof(revid));
        }
        
        default:
            printf("Unknown vendor command: 0x%02X\n", request->bRequest);
            return false;
    }
}

// Handle vendor control data stage
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const* request) {
    (void) rhport;
    
    if (request->bRequest == CMD_START && request->bmRequestType_bit.direction == TUSB_DIR_OUT) {
        if (request->wLength == sizeof(struct cmd_start_acquisition)) {
            // Command data should now be available in pending_cmd
            printf("START command processing:\n");
            printf("  flags = 0x%02X\n", pending_cmd.flags);
            printf("  sample_delay = 0x%02X%02X\n", pending_cmd.sample_delay_h, pending_cmd.sample_delay_l);
            
            // Decode flags
            sample_16bit = (pending_cmd.flags & (1 << CMD_START_FLAGS_WIDE_POS)) != 0;
            bool clk_48mhz = (pending_cmd.flags & (1 << CMD_START_FLAGS_CLK_SRC_POS)) != 0;
            
            printf("  Sample mode: %s\n", sample_16bit ? "16-bit" : "8-bit");
            printf("  Clock: %s\n", clk_48mhz ? "48MHz" : "30MHz");
            
            // Start sampling
            buffer_pos = 0;
            sample_count = 0;
            sampling_active = true;
            
            printf("Sampling started!\n");
        }
    }
    return true;
}

// Handle control data (called when data is received)
bool tud_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    (void) rhport;
    
    if (stage == CONTROL_STAGE_DATA && request->bRequest == CMD_START) {
        // Copy the received data to our buffer
        if (request->wLength == sizeof(struct cmd_start_acquisition)) {
            // The data is available in the control endpoint buffer
            // For TinyUSB, we need to handle this differently
            return true;
        }
    }
    return true;
}

// Main vendor task for sending data
void vendor_task(void) {
    // Send collected data when USB is ready
    if (buffer_pos > 0 && tud_vendor_available()) {
        uint32_t bytes_to_send = buffer_pos;
        if (bytes_to_send > USB_EP_SIZE) {
            bytes_to_send = USB_EP_SIZE;
        }
        
        uint32_t bytes_sent = tud_vendor_write(sample_buffer, bytes_to_send);
        if (bytes_sent > 0) {
            printf("Sent %d bytes to USB\n", bytes_sent);
            
            // Shift remaining data
            if (bytes_sent < buffer_pos) {
                memmove(sample_buffer, sample_buffer + bytes_sent, buffer_pos - bytes_sent);
                buffer_pos -= bytes_sent;
            } else {
                buffer_pos = 0;
            }
            
            tud_vendor_flush();
        }
    }
}

// USB device connected callback
void tud_mount_cb(void) {
    printf("USB Device mounted!\n");
}

// USB device disconnected callback
void tud_umount_cb(void) {
    printf("USB Device unmounted!\n");
}

int main() {
    // Initialize standard library
    stdio_init_all();
    
    printf("\n");
    printf("======================================\n");
    printf("fx2lafw Compatible Logic Analyzer\n");
    printf("Raspberry Pi Pico Implementation\n");
    printf("======================================\n");
    printf("Version: %d.%d\n", FX2LAFW_VERSION_MAJOR, FX2LAFW_VERSION_MINOR);
    printf("USB VID:PID = 1D50:608C (fx2lafw compatible)\n");
    printf("GPIO pins: %d-%d (GPIO 0,1 reserved for UART)\n", SAMPLE_PIN_BASE, SAMPLE_PIN_BASE + SAMPLE_PIN_COUNT - 1);
    printf("\n");
    
    // Setup GPIO pins for sampling
    setup_gpio_sampling();
    
    // Initialize USB
    printf("Initializing USB...\n");
    tusb_init();
    
    printf("Ready! Connect with PulseView using fx2lafw driver.\n");
    printf("Commands supported:\n");
    printf("  CMD_GET_FW_VERSION (0x%02X)\n", CMD_GET_FW_VERSION);
    printf("  CMD_START (0x%02X)\n", CMD_START);
    printf("  CMD_GET_REVID_VERSION (0x%02X)\n", CMD_GET_REVID_VERSION);
    printf("\n");
    
    while (true) {
        // Handle USB events
        tud_task();
        
        // Generate test data when sampling is active
        if (sampling_active) {
            generate_test_data();
            sleep_ms(1); // 1ms delay for test data generation
        }
        
        // Handle data transmission
        vendor_task();
        
        // Small delay
        sleep_us(100);
    }
}
