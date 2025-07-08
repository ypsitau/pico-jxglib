#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "jxglib/SPI.h"

// SPI pins definition (adjust according to your hardware setup)
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

int main()
{
    stdio_init_all();

    // Initialize SPI pins
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Initialize CS pin as output
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // CS high (inactive)

    // Get SPI instance and initialize
    jxglib::SPI& spi = jxglib::SPI::get_instance(0); // SPI0
    uint actual_baudrate = spi.init(1000000); // 1MHz
    
    // Set SPI format: 8 bits, CPOL=0, CPHA=0, MSB first
    spi.set_format(8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    printf("SPI initialized at %u Hz\n", actual_baudrate);

    while (true) {
        // Example SPI communication
        uint8_t tx_data[] = {0x01, 0x02, 0x03, 0x04};
        uint8_t rx_data[4];
        
        // Pull CS low to start communication
        gpio_put(PIN_CS, 0);
        
        // Write and read data simultaneously
        int result = spi.WriteRead(tx_data, rx_data, sizeof(tx_data));
        
        // Pull CS high to end communication
        gpio_put(PIN_CS, 1);
        
        if (result == sizeof(tx_data)) {
            printf("SPI transaction successful:\n");
            printf("TX: ");
            for (int i = 0; i < sizeof(tx_data); i++) {
                printf("0x%02X ", tx_data[i]);
            }
            printf("\nRX: ");
            for (int i = 0; i < sizeof(rx_data); i++) {
                printf("0x%02X ", rx_data[i]);
            }
            printf("\n\n");
        } else {
            printf("SPI transaction failed\n");
        }
        
        sleep_ms(1000);
    }
}
