#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7789.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/Font/shinonome16.h"
#include "jxglib/Font/sisd24x32.h"

using namespace jxglib;

void Test_SSD1306()
{
	::i2c_init(i2c0, 400000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	SSD1306 display(i2c0);
	display.Initialize();
}

void Test_ST7789()
{
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	display.Initialize();
}

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	SSD1306 display(i2c0);
	display.Initialize();
	Terminal terminal;
	terminal.AttachOutput(display);
	terminal.SetFont(Font::shinonome12);
	//terminal.SetFont(Font::shinonome16, 2);
	//terminal.SetFont(Font::sisd24x32);
	for (int i = 0; i < 1000; i++) {
		terminal.printf("hoge %d\n", i);
		::sleep_ms(1000);
	}
}
