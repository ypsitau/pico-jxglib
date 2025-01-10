#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/ILI9488.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9488 display(spi1, 320, 480, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	//ILI9488 display(spi1, 320, 480, {RST: GPIO19, DC: GPIO18, CS: GPIO20, BL: GPIO21});
	display.Initialize(Display::Dir::Rotate0);
	//Drawable_TestCase::DrawString(display);
	//Drawable_TestCase::RotateImage(display);
	Drawable_TestCase::DrawFonts(display);
}
