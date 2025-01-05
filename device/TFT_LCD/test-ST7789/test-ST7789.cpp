#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/ST7789.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	::printf("%d\n", sizeof(FontEntry));
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	Drawable_TestCase::DrawString(display);
	//Drawable_TestCase::RotateImage(display);
}
