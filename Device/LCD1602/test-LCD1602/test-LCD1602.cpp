#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LCD1602.h"

using namespace jxglib;

void test_PrintAllChars(LCD1602& lcd)
{
	const char* strTbl[] = {
		" !\"#$%&'()*+,-./"
		"0123456789:;<=>?",
		"@ABCDEFGHIJKLMNO"
		"PQRSTUVWXYZ[\\]^_",
		"`abcdefghijklmno"
		"pqrstuvwxyz{|}~\x7f",
		"\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf"
		"\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf",
		"\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf"
		"\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf",
		"\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef"
		"\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff",
	};
	for (;;) {
		for (int i = 0; i < count_of(strTbl); i++) {
			lcd.ClearScreen();
			lcd.Print(strTbl[i]);
			::sleep_ms(1000);
		}
	}
}

void test_Scroll(LCD1602& lcd)
{
	lcd.ClearScreen();
	for (int i = 0; i < 100; i++) {
		lcd.Printf("Hello %d\n", i);
		::sleep_ms(100);
	}
}

void test_Indicator(LCD1602& lcd)
{
	lcd.ClearScreen();
	for (int i = 0; i <= 16; i++) {
		lcd.Printf("%d\n", 16 - i);
		::sleep_ms(100);
	}
}

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	LCD1602 lcd(i2c0);
	lcd.Initialize();
	//lcd.raw.FunctionSet(0, 0, 1);
	//test_PrintAllChars(lcd);
	//test_Scroll(lcd);
	test_Indicator(lcd);
}
