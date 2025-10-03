#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125'000'000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
	display.Initialize(Display::Dir::Rotate0);
	display.SetFont(Font::shinonome16);
	touchScreen.Initialize(display);
	touchScreen.Calibrate(display, true);
	touchScreen.PrintCalibration();
	for (;;) {
		if (Tickable::Tick(10)) {
			Point pt;
			if (touchScreen.ReadPoint(&pt)) display.DrawRectFill(pt.x - 1, pt.y - 1, 2, 2);
		}
	}
}
