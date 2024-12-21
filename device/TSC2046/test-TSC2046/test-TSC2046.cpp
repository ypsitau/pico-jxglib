#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TSC2046.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	//::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi0, 1 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	TSC2046 touch(spi0, GPIO6, GPIO7);
	touch.Initialize();
	for (;;) {
		uint16_t x, y;
		bool touched = touch.ReadPosition(&x, &y);
		if (touched) ::printf("%4x %4x\n", x, y);
		::sleep_ms(100);
	}
}
