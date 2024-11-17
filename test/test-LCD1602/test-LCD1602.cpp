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
	//lcd.raw.FunctionSet(0, 0, 1);
	const char* str =
		" !\"#$%&'()*+,-./"
		"0123456789:;<=>?"
		"@ABCDEFGHIJKLMNO"
		"PQRSTUVWXYZ[\\]^_"
		"`abcdefghijklmno"
		"pqrstuvwxyz{~}";
	lcd.Print(str);
	//lcd.SetPosition(0, 1);
	//lcd.Print("Hello World");
}
