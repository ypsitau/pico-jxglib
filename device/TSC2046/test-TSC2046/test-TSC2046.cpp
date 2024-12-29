#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TSC2046.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);	// 2MHz seems the max SPI frequency for TSC2046
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	TSC2046 touchScreen(spi0, {CS: GPIO6, IRQ: GPIO7});
	touchScreen.Initialize();
	for (;;) {
		int x, y;
		int z1, z2;
		bool touched = touchScreen.ReadPositionRaw(&x, &y, &z1, &z2);
		if (touched) ::printf("x:%-4x y:%-4x z1:%-2x z2:%-2x\n", x, y, z1, z2);
		::sleep_ms(100);
	}
}
