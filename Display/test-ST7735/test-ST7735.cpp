#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7735.h"
#include "jxglib/DrawableTest.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7735 display(spi1, 80, 160, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	//ST7735 display(spi1, 128, 160, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	//ST7735::TypeB display(spi1, 128, 160, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	//DrawableTest::DrawString(display);
	DrawableTest::RotateImage(display);
}
