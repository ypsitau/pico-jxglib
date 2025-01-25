#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/DrawableTest.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c1, 400000);
	GPIO6.set_function_I2C1_SDA().pull_up();
	GPIO7.set_function_I2C1_SCL().pull_up();
	SSD1306 display(i2c1);
	display.Initialize();
	DrawableTest::DrawString(display);
	//DrawableTest::RotateImage(display);
	//DrawableTest::DrawFonts(display);
}
