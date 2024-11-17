#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LCD1602.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	LCD1602 lcd(i2c0);
	lcd.Initialize();
	lcd.Print("Hello World");
	lcd.Print("Hello World");
}
