#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TSC2046.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 1 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	TSC2046 touchScreen(spi0, GPIO6, GPIO7);
	touchScreen.Initialize();
	for (;;) {
		int x, y;
		int z1, z2;
		bool touched = touchScreen.ReadPosition(&x, &y, &z1, &z2);
		if (touched) ::printf("%4x %4x %2x %2x %d\n", x, y, z1, z2, (z2 / z1 - 1) * x * 100 / 4096);
		::sleep_ms(100);
	}
}
