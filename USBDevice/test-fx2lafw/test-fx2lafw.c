/*
 * fx2lafw compatible logic analyzer implementation for Raspberry Pi Pico
 * Minimal implementation for USB communication reference with PulseView
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "tusb.h"
#include "device/usbd_pvt.h"

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
    static uint32_t last_debug_time = 0;
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
    
    // Debug message every 25 samples
    if (sample_count % 25 == 0) {
        printf("Test data generation: Sample %d, Buffer: %d bytes, Value: 0x%02X\n", 
               sample_count, buffer_pos, test_value);
    }
    
    // Stop after collecting some samples
    if (sample_count > 100) {
        sampling_active = false;
        printf("=== TEST SAMPLING COMPLETED ===\n");
        printf("Total samples collected: %d\n", sample_count);
        printf("Final buffer size: %d bytes\n", buffer_pos);
    }
}

// Handle vendor control requests (fx2lafw compatible)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    printf("USB Control Callback: rhport=%d, stage=%d ", rhport, stage);
    
    switch (stage) {
        case CONTROL_STAGE_SETUP:
            printf("(SETUP)\n");
            break;
        case CONTROL_STAGE_DATA:
            printf("(DATA)\n");
            break;
        case CONTROL_STAGE_ACK:
            printf("(ACK)\n");
            break;
        default:
            printf("(UNKNOWN)\n");
            break;
    }
    
    // Handle all stages, not just SETUP
    if (stage == CONTROL_STAGE_SETUP) {
        printf("USB Control Request (SETUP stage):\n");
        printf("  bmRequestType = 0x%02X\n", request->bmRequestType);
        printf("  bRequest = 0x%02X\n", request->bRequest);
        printf("  wValue = 0x%04X\n", request->wValue);
        printf("  wIndex = 0x%04X\n", request->wIndex);
        printf("  wLength = %d\n", request->wLength);
        printf("  Direction: %s\n", request->bmRequestType_bit.direction ? "IN (device to host)" : "OUT (host to device)");
        
        switch (request->bRequest) {
            case CMD_GET_FW_VERSION: {
                printf("Processing CMD_GET_FW_VERSION\n");
                struct version_info version = {
                    .major = FX2LAFW_VERSION_MAJOR,
                    .minor = FX2LAFW_VERSION_MINOR
                };
                printf("Sending firmware version: %d.%d\n", version.major, version.minor);
                bool result = tud_control_xfer(rhport, request, &version, sizeof(version));
                printf("tud_control_xfer result: %s\n", result ? "SUCCESS" : "FAILED");
                return result;
            }
            
            case CMD_START: {
                printf("Processing CMD_START command\n");
                printf("  Expected data size: %d bytes\n", sizeof(struct cmd_start_acquisition));
                printf("  Received wLength: %d bytes\n", request->wLength);
                
                if (request->wLength == sizeof(struct cmd_start_acquisition)) {
                    printf("  Data size matches, setting up data transfer\n");
                    // Read the command data directly
                    bool result = tud_control_xfer(rhport, request, &pending_cmd, sizeof(pending_cmd));
                    printf("  tud_control_xfer result: %s\n", result ? "SUCCESS" : "FAILED");
                    return result;
                } else {
                    printf("  ERROR: Data size mismatch! Expected %d, got %d\n", 
                           sizeof(struct cmd_start_acquisition), request->wLength);
                    return false;
                }
            }
            
            case CMD_GET_REVID_VERSION: {
                printf("Processing CMD_GET_REVID_VERSION\n");
                uint16_t revid = 0x0100; // Revision ID
                printf("Sending revision ID: 0x%04X\n", revid);
                bool result = tud_control_xfer(rhport, request, &revid, sizeof(revid));
                printf("tud_control_xfer result: %s\n", result ? "SUCCESS" : "FAILED");
                return result;
            }
            
            default:
                printf("ERROR: Unknown vendor command: 0x%02X\n", request->bRequest);
                printf("  This command is not supported\n");
                return false;
        }
    } else if (stage == CONTROL_STAGE_DATA) {
        printf("  Handling DATA stage for request 0x%02X\n", request->bRequest);
        
        // For CMD_START, process the received data immediately
        if (request->bRequest == CMD_START) {
            printf("  Processing START command data in DATA stage\n");
            printf("  Data received in pending_cmd:\n");
            printf("    flags = 0x%02X\n", pending_cmd.flags);
            printf("    sample_delay_h = 0x%02X\n", pending_cmd.sample_delay_h);
            printf("    sample_delay_l = 0x%02X\n", pending_cmd.sample_delay_l);
            printf("    Full delay value = 0x%04X\n", (pending_cmd.sample_delay_h << 8) | pending_cmd.sample_delay_l);
            
            // Decode flags in detail
            sample_16bit = (pending_cmd.flags & (1 << CMD_START_FLAGS_WIDE_POS)) != 0;
            bool clk_48mhz = (pending_cmd.flags & (1 << CMD_START_FLAGS_CLK_SRC_POS)) != 0;
            
            printf("  Flag analysis:\n");
            printf("    Bit %d (WIDE): %d -> Sample mode: %s\n", 
                   CMD_START_FLAGS_WIDE_POS, 
                   (pending_cmd.flags >> CMD_START_FLAGS_WIDE_POS) & 1,
                   sample_16bit ? "16-bit" : "8-bit");
            printf("    Bit %d (CLK_SRC): %d -> Clock: %s\n", 
                   CMD_START_FLAGS_CLK_SRC_POS,
                   (pending_cmd.flags >> CMD_START_FLAGS_CLK_SRC_POS) & 1,
                   clk_48mhz ? "48MHz" : "30MHz");
            
            // Start sampling
            buffer_pos = 0;
            sample_count = 0;
            sampling_active = true;
            
            printf("  === SAMPLING STARTED IN DATA STAGE ===\n");
            printf("  Ready to generate and send test data\n");
        }
        
        return true;
    } else if (stage == CONTROL_STAGE_ACK) {
        printf("  Handling ACK stage for request 0x%02X\n", request->bRequest);
        
        // Double-check sampling activation for START command
        if (request->bRequest == CMD_START && !sampling_active) {
            printf("  WARNING: ACK stage but sampling not active, activating now\n");
            
            // Decode flags again as fallback
            sample_16bit = (pending_cmd.flags & (1 << CMD_START_FLAGS_WIDE_POS)) != 0;
            bool clk_48mhz = (pending_cmd.flags & (1 << CMD_START_FLAGS_CLK_SRC_POS)) != 0;
            
            printf("  Fallback flag analysis:\n");
            printf("    flags = 0x%02X\n", pending_cmd.flags);
            printf("    Sample mode: %s\n", sample_16bit ? "16-bit" : "8-bit");
            printf("    Clock: %s\n", clk_48mhz ? "48MHz" : "30MHz");
            
            // Start sampling as fallback
            buffer_pos = 0;
            sample_count = 0;
            sampling_active = true;
            
            printf("  === SAMPLING STARTED IN ACK STAGE (FALLBACK) ===\n");
        }
        
        return true;
    }
    
    printf("  Default return true for stage %d\n", stage);
    return true;
}

// Handle vendor control data stage
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const* request) {
    printf("USB Control Complete Callback:\n");
    printf("  rhport = %d\n", rhport);
    printf("  bRequest = 0x%02X\n", request->bRequest);
    printf("  bmRequestType = 0x%02X\n", request->bmRequestType);
    printf("  Direction: %s\n", request->bmRequestType_bit.direction ? "IN" : "OUT");
    printf("  wLength = %d\n", request->wLength);
    
    if (request->bRequest == CMD_START && request->bmRequestType_bit.direction == TUSB_DIR_OUT) {
        printf("Processing START command data completion\n");
        
        if (request->wLength == sizeof(struct cmd_start_acquisition)) {
            // Command data should now be available in pending_cmd
            printf("START command data received successfully:\n");
            printf("  flags = 0x%02X\n", pending_cmd.flags);
            printf("  sample_delay_h = 0x%02X\n", pending_cmd.sample_delay_h);
            printf("  sample_delay_l = 0x%02X\n", pending_cmd.sample_delay_l);
            printf("  Full delay value = 0x%04X\n", (pending_cmd.sample_delay_h << 8) | pending_cmd.sample_delay_l);
            
            // Decode flags in detail
            sample_16bit = (pending_cmd.flags & (1 << CMD_START_FLAGS_WIDE_POS)) != 0;
            bool clk_48mhz = (pending_cmd.flags & (1 << CMD_START_FLAGS_CLK_SRC_POS)) != 0;
            
            printf("Flag analysis:\n");
            printf("  Bit %d (WIDE): %d -> Sample mode: %s\n", 
                   CMD_START_FLAGS_WIDE_POS, 
                   (pending_cmd.flags >> CMD_START_FLAGS_WIDE_POS) & 1,
                   sample_16bit ? "16-bit" : "8-bit");
            printf("  Bit %d (CLK_SRC): %d -> Clock: %s\n", 
                   CMD_START_FLAGS_CLK_SRC_POS,
                   (pending_cmd.flags >> CMD_START_FLAGS_CLK_SRC_POS) & 1,
                   clk_48mhz ? "48MHz" : "30MHz");
            
            // Start sampling
            buffer_pos = 0;
            sample_count = 0;
            sampling_active = true;
            
            printf("=== SAMPLING STARTED ===\n");
            printf("Ready to generate and send test data\n");
        } else {
            printf("ERROR: Wrong data size for START command\n");
            printf("  Expected: %d bytes\n", sizeof(struct cmd_start_acquisition));
            printf("  Received: %d bytes\n", request->wLength);
        }
    } else {
        printf("Control complete for other command or direction\n");
    }
    
    return true;
}

// Handle control data (called when data is received)
bool tud_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
    printf("Control Transfer Callback:\n");
    printf("  rhport = %d\n", rhport);
    printf("  stage = %d ", stage);
    
    switch (stage) {
        case CONTROL_STAGE_SETUP:
            printf("(SETUP)\n");
            break;
        case CONTROL_STAGE_DATA:
            printf("(DATA)\n");
            break;
        case CONTROL_STAGE_ACK:
            printf("(ACK)\n");
            break;
        default:
            printf("(UNKNOWN)\n");
            break;
    }
    
    printf("  bmRequestType = 0x%02X\n", request->bmRequestType);
    printf("  bRequest = 0x%02X\n", request->bRequest);
    printf("  wLength = %d\n", request->wLength);
    
    // Check if this is a vendor request
    if ((request->bmRequestType_bit.type == TUSB_REQ_TYPE_VENDOR) && 
        (request->bmRequestType_bit.recipient == TUSB_REQ_RCPT_DEVICE)) {
        printf("  This is a VENDOR request for DEVICE\n");
        
        if (stage == CONTROL_STAGE_SETUP) {
            printf("  Handling vendor request in standard control callback\n");
            
            switch (request->bRequest) {
                case CMD_GET_FW_VERSION: {
                    printf("  CMD_GET_FW_VERSION in standard callback\n");
                    struct version_info version = {
                        .major = FX2LAFW_VERSION_MAJOR,
                        .minor = FX2LAFW_VERSION_MINOR
                    };
                    return tud_control_xfer(rhport, request, &version, sizeof(version));
                }
                
                case CMD_START: {
                    printf("  CMD_START in standard callback\n");
                    if (request->wLength == sizeof(struct cmd_start_acquisition)) {
                        return tud_control_xfer(rhport, request, &pending_cmd, sizeof(pending_cmd));
                    }
                    return false;
                }
                
                case CMD_GET_REVID_VERSION: {
                    printf("  CMD_GET_REVID_VERSION in standard callback\n");
                    uint16_t revid = 0x0100;
                    return tud_control_xfer(rhport, request, &revid, sizeof(revid));
                }
                
                default:
                    printf("  Unknown vendor request: 0x%02X\n", request->bRequest);
                    return false;
            }
        }
    }
    
    if (stage == CONTROL_STAGE_DATA && request->bRequest == CMD_START) {
        printf("Handling START command data stage in standard callback\n");
        // Copy the received data to our buffer
        if (request->wLength == sizeof(struct cmd_start_acquisition)) {
            printf("Data stage for START command with correct size\n");
            
            // Process the START command immediately
            printf("Processing START command in data stage:\n");
            printf("  flags = 0x%02X\n", pending_cmd.flags);
            printf("  sample_delay_h = 0x%02X\n", pending_cmd.sample_delay_h);
            printf("  sample_delay_l = 0x%02X\n", pending_cmd.sample_delay_l);
            
            // Decode flags
            sample_16bit = (pending_cmd.flags & (1 << CMD_START_FLAGS_WIDE_POS)) != 0;
            bool clk_48mhz = (pending_cmd.flags & (1 << CMD_START_FLAGS_CLK_SRC_POS)) != 0;
            
            printf("  Sample mode: %s\n", sample_16bit ? "16-bit" : "8-bit");
            printf("  Clock: %s\n", clk_48mhz ? "48MHz" : "30MHz");
            
            // Start sampling
            buffer_pos = 0;
            sample_count = 0;
            sampling_active = true;
            
            printf("=== SAMPLING STARTED IN STANDARD CALLBACK ===\n");
            
            return true;
        } else {
            printf("ERROR: Data stage size mismatch\n");
        }
    }
    
    return false; // Let TinyUSB handle other requests
}

// Main vendor task for sending data
void vendor_task(void) {
    // Send collected data when USB is ready
    if (buffer_pos > 0) {
        // Check USB vendor interface availability
        bool vendor_available = tud_vendor_available();
        bool mounted = tud_mounted();
        
        static uint32_t last_debug = 0;
        uint32_t current_time = time_us_32() / 1000;
        
        // Debug USB state every 2 seconds when buffer has data
        if (current_time - last_debug > 2000) {
            printf("USB State Debug:\n");
            printf("  tud_mounted(): %s\n", mounted ? "YES" : "NO");
            printf("  tud_vendor_available(): %s\n", vendor_available ? "YES" : "NO");
            printf("  Buffer size: %d bytes\n", buffer_pos);
            last_debug = current_time;
        }
        
        // Try to send data if any USB condition is met
#if 0
        if (vendor_available || mounted) {
            uint32_t bytes_to_send = buffer_pos;
            if (bytes_to_send > USB_EP_SIZE) {
                bytes_to_send = USB_EP_SIZE;
            }
            
            printf("Vendor task: Attempting to send %d bytes (buffer has %d bytes)\n", 
                   bytes_to_send, buffer_pos);
            printf("  USB conditions: available=%s, mounted=%s\n",
                   vendor_available ? "Y" : "N",
                   mounted ? "Y" : "N");
            
            uint32_t bytes_sent = tud_vendor_write(sample_buffer, bytes_to_send);
            if (bytes_sent > 0) {
                printf("SUCCESS: Sent %d bytes to USB\n", bytes_sent);
                
                // Show first few bytes of sent data
                printf("Data sent: ");
                for (uint32_t i = 0; i < bytes_sent && i < 8; i++) {
                    printf("0x%02X ", sample_buffer[i]);
                }
                if (bytes_sent > 8) {
                    printf("...");
                }
                printf("\n");
                
                // Shift remaining data
                if (bytes_sent < buffer_pos) {
                    memmove(sample_buffer, sample_buffer + bytes_sent, buffer_pos - bytes_sent);
                    buffer_pos -= bytes_sent;
                    printf("Buffer shifted, remaining: %d bytes\n", buffer_pos);
                } else {
                    buffer_pos = 0;
                    printf("Buffer emptied\n");
                }
                
                bool flush_result = tud_vendor_flush();
                printf("USB flush result: %s\n", flush_result ? "SUCCESS" : "FAILED");
            } else {
                printf("WARNING: Failed to send data (tud_vendor_write returned 0)\n");
                
                // Try alternative approach: force send without availability check
                static int retry_count = 0;
                if (retry_count++ < 3) {
                    printf("  Attempting forced send (retry %d/3)\n", retry_count);
                    // Force a smaller packet
                    bytes_to_send = (bytes_to_send > 16) ? 16 : bytes_to_send;
                    bytes_sent = tud_vendor_write(sample_buffer, bytes_to_send);
                    if (bytes_sent > 0) {
                        printf("  Forced send SUCCESS: %d bytes\n", bytes_sent);
                        // Shift buffer
                        if (bytes_sent < buffer_pos) {
                            memmove(sample_buffer, sample_buffer + bytes_sent, buffer_pos - bytes_sent);
                            buffer_pos -= bytes_sent;
                        } else {
                            buffer_pos = 0;
                        }
                        retry_count = 0; // Reset retry count on success
                    }
                } else {
                    retry_count = 0; // Reset after max retries
                }
            }
        } else {
            static uint32_t last_unavailable_msg = 0;
            if (current_time - last_unavailable_msg > 3000) { // Every 3 seconds
                printf("Vendor task: Buffer has %d bytes but USB not available\n", buffer_pos);
                last_unavailable_msg = current_time;
            }
        }
#endif
    }
}

// USB device connected callback
void tud_mount_cb(void) {
    printf("=== USB DEVICE MOUNTED ===\n");
    printf("Device is now connected to host\n");
}

// USB device disconnected callback
void tud_umount_cb(void) {
    printf("=== USB DEVICE UNMOUNTED ===\n");
    printf("Device disconnected from host\n");
    
    // Stop sampling when disconnected
    sampling_active = false;
    buffer_pos = 0;
}

// Vendor class callbacks
void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize) {
    printf("Vendor RX callback: interface=%d, size=%d\n", itf, bufsize);
    (void) itf;
    (void) buffer;
    (void) bufsize;
}

void tud_vendor_tx_cb(uint8_t itf, uint32_t sent_bytes) {
    printf("Vendor TX callback: interface=%d, sent=%d bytes\n", itf, sent_bytes);
    (void) itf;
    (void) sent_bytes;
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
    printf("=== ENTERING MAIN LOOP ===\n");
    printf("Waiting for USB commands...\n");
    
    uint32_t loop_counter = 0;
    uint32_t last_status_time = 0;
    
    while (true) {
        loop_counter++;
        uint32_t current_time = time_us_32() / 1000; // Convert to milliseconds
        
        // Print status every 5 seconds
        if (current_time - last_status_time > 5000) {
            printf("Status: Loop %d, USB connected: %s, Sampling: %s, Buffer: %d bytes\n",
                   loop_counter,
                   tud_mounted() ? "YES" : "NO",
                   sampling_active ? "ACTIVE" : "IDLE",
                   buffer_pos);
            
            if (sampling_active) {
                printf("  Sampling details: Count=%d, 16bit=%s\n", 
                       sample_count, sample_16bit ? "YES" : "NO");
            }
            
            last_status_time = current_time;
        }
        
        // Handle USB events
        tud_task();
        
        // Generate test data when sampling is active
        if (sampling_active) {
            //==================================================================================================================
            printf("--------------------------------------------------\n");
            printf("Generating test data...\n");
            for (int i = 0; i < 256; i++) {
                sample_buffer[i * 2] = 0xff;
                sample_buffer[i * 2 + 1] = 0x00;
            }
            for (int bytes = 0; bytes < 512; ) {
                int bytes_sent = tud_vendor_write(sample_buffer + bytes, 512 - bytes);
                printf("Sent %d bytes of test data\n", bytes_sent);
                if (bytes_sent <= 0) {
                    printf("ERROR: Failed to send test data, stopping sampling\n");
                    sampling_active = false;
                    break;
                }
                bytes += bytes_sent;
            }
            sampling_active = false; // Reset sampling after sending data
            //generate_test_data();
            sleep_ms(1); // 1ms delay for test data generation
        } else {
            // Check if we should start sampling based on pending_cmd
            static uint32_t last_check = 0;
            if (current_time - last_check > 1000) { // Check every second
                if (pending_cmd.flags != 0 && !sampling_active) {
                    printf("DEBUG: Found pending command but sampling not active\n");
                    printf("  pending_cmd.flags = 0x%02X\n", pending_cmd.flags);
                    printf("  Attempting to start sampling...\n");
                    
                    sample_16bit = (pending_cmd.flags & (1 << CMD_START_FLAGS_WIDE_POS)) != 0;
                    buffer_pos = 0;
                    sample_count = 0;
                    sampling_active = true;
                    
                    printf("  === SAMPLING STARTED (RECOVERY) ===\n");
                }
                last_check = current_time;
            }
        }
        
// Handle data transmission
        vendor_task();
        
        // Additional USB processing for vendor endpoints
        if (buffer_pos > 0) {
            // Force USB processing to ensure endpoints are ready
            tud_task(); // Extra call to process USB events
        }
        
        // Small delay
        sleep_us(100);
    }
}
