#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/SSD1306.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c1, 400000);
	GPIO26.set_function_I2C1_SDA().pull_up();
	GPIO27.set_function_I2C1_SCL().pull_up();
	SSD1306 display(i2c1);
	display.Initialize();
	Drawable_TestCase::DrawString(display);
	//Drawable_TestCase::RotateImage(display);
	//Drawable_TestCase::DrawFonts(display);
}
