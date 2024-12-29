#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/ST7735.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO15.set_function_SPI1_TX().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	ST7789 display1(spi0, 240, 240, {RST: GPIO6, DC: GPIO7, BL: GPIO::None});
	ST7789 display2(spi1, 240, 320, {RST: GPIO8, DC: GPIO9, CS: GPIO10, BL: GPIO::None});
	ILI9341 display3(spi1, 240, 320, {RST: GPIO12, DC: GPIO11, CS: GPIO13, BL: GPIO::None});
	ST7735 display4(spi1, 80, 160, {RST: GPIO16, DC: GPIO17, CS: GPIO18, BL: GPIO::None});
	ST7735::TypeB display5(spi1, 128, 160, {RST: GPIO19, DC: GPIO20, CS: GPIO21, BL: GPIO::None});
	Display::Dir displayDir = Display::Dir::Rotate0;
	//Display::Dir displayDir = Display::Dir::Rotate90;
	//Display::Dir displayDir = Display::Dir::Rotate180;
	//Display::Dir displayDir = Display::Dir::Rotate270;
	display1.Initialize(displayDir);
	display2.Initialize(displayDir);
	display3.Initialize(displayDir);
	display4.Initialize(displayDir);
	display5.Initialize(displayDir);
	Drawable* drawables[] = { &display3 };
	//Drawable* drawables[] = { &display1, &display2, &display4, &display5 };
	Drawable_TestCase::DrawString(drawables, count_of(drawables));
	//Drawable_TestCase::RotateImage(drawables, count_of(drawables));
}
