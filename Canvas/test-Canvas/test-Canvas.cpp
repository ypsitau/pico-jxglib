#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7735.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/Canvas.h"
#include "jxglib/Drawable_TestCase.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	//ILI9341 display(spi1, 240, 320, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	Display::Dir displayDir = Display::Dir::Rotate0;
	display.Initialize(displayDir);
	Canvas canvas;
	canvas.AttachOutput(display, Canvas::DrawDir::Rotate0, {50, 50, 100, 150});
	Drawable_TestCase::DrawString(canvas);
	//Drawable_TestCase::RotateImage(canvas);
}
