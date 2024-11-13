#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/ST7789.h"

using namespace jxglib;

int main()
{
	stdio_init_all();
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	::gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	ST7789 screen(spi0, 240, 240, 20, 21, 22);
	screen.Initialize();
	screen.DrawRectFill(0, 0, 100, 100);
}
