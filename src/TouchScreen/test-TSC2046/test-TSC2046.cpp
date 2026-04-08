#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TouchScreen/TSC2046.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);	// 2MHz seems the max SPI frequency for TSC2046
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	TouchScreen::TSC2046 touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
	touchScreen.Initialize();
	::printf("Touch Screen Test\n");
	for (;;) {
		Tickable::Sleep(100);
		int x, y, z1, z2;
		bool touched = touchScreen.ReadXYRaw(&x, &y, &z1, &z2);
		if (touched) ::printf("x:%-4x y:%-4x z1:%-2x z2:%-2x\n", x, y, z1, z2);
	}
}
