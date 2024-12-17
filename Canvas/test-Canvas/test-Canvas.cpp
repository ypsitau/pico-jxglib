#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/Canvas.h"
#include "jxglib/Drawable_TestCase.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	GPIO15.set_function_SPI1_TX().set_drive_strength(GPIO_DRIVE_STRENGTH_12MA);
	ILI9341 display(spi1, 240, 320, GPIO11, GPIO12, GPIO13, GPIO::None);
	Display::Dir displayDir = Display::Dir::Rotate0;
	display.Initialize(displayDir);
	Canvas canvas;
	canvas.AttachOutput(display, Canvas::DrawDir::Rotate0);
	//canvas.AttachOutput(display, {50, 50, 100, 150}, Canvas::AttachDir::Rotate0);
	Drawable* drawableTbl[] = { &canvas };
	//Drawable_TestCase::DrawString(drawableTbl, count_of(drawableTbl));
	Drawable_TestCase::RotateImage(drawableTbl, count_of(drawableTbl));
}
