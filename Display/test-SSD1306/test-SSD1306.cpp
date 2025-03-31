#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/DrawableTest.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	SSD1306 display(i2c0);
	display.Initialize();
	DrawableTest::DrawString(display);
	//DrawableTest::RotateImage(display);
	//DrawableTest::DrawFonts(display);
}
