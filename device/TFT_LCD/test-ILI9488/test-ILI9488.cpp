#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/ILI9488.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9488 display(spi1, 320, 480, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	ILI9488::TouchScreen touchScreen(spi0, {CS: GPIO6, IRQ: GPIO7});
	display.Initialize(Display::Dir::Rotate0);
	touchScreen.Initialize(display);
	Drawable_TestCase::DrawString(display);
	//Drawable_TestCase::RotateImage(display);
	//for (;;) ;
#if 1
	//Drawable_TestCase::RotateImage(display);
	//touchScreen.Calibrate(display);
	::printf("----\n");
	touchScreen.PrintCalibration();
	for (;;) {
		int x, y;
		if (touchScreen.ReadPosition(&x, &y)) {
			display.DrawRectFill(x - 2, y - 2, 4, 4);
		}
		::sleep_ms(10);
	}
#endif
}
