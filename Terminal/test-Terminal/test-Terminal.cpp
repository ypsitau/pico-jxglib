#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7735.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
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
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO8.set_function_I2C0_SDA().pull_up();
	GPIO9.set_function_I2C0_SCL().pull_up();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	SSD1306 display0(i2c0);
	ST7789 display1(spi0, 240, 240, GPIO4, GPIO5, GPIO6);
	ST7789 display2(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	ILI9341 display3(spi1, 240, 320, GPIO16, GPIO17, GPIO18, GPIO19);
	ST7735 display4(spi1, 80, 160, GPIO20, GPIO21, GPIO22, GPIO23);
	ST7735::TypeB display5(spi1, 130, 161, GPIO24, GPIO25, GPIO26, GPIO27);
	display0.Initialize();
	display1.Initialize();
	display2.Initialize();
	display3.Initialize();
	display4.Initialize();
	display5.Initialize();
	Terminal terminal;
	terminal.AttachOutput(display2);
	//terminal.AttachOutput(display2, Terminal::AttachDir::Rotate90);
	//terminal.AttachOutput(display3);
	//terminal.AttachOutput(display4, Terminal::AttachDir::Rotate90);
	terminal.SetFont(Font::shinonome12);
	//terminal.SetFont(Font::shinonome16);
	//terminal.SetFont(Font::sisd24x32);
	//terminal.SetColorBg(Color::green);
	terminal.Clear();
	//terminal.SetFont(Font::shinonome16);
#if 1
	terminal.dumpStyle.DigitsAddr(4).Cols((terminal.GetColNum() - 4 - 1) / 3);
	terminal.Dump(reinterpret_cast<const void*>(0), 8 * 2000);
#endif
#if 0
	for (int i = 0; i < 40; i++) {
		terminal.Locate(i, i).Puts("Hi").Flush();
	}
#endif
#if 0
	//terminal.Locate(0, 20);
	for (int i = 0; ; i++) {
		terminal.Printf("Count: %d\r", i);
		::sleep_ms(500);
	}
#endif
	//terminal.SetFont(Font::sisd24x32);
	//for (int i = 0; i < 1000; i++) {
	//	terminal.printf("hoge %d\n", i);
	//}
	//display.DrawChar({0, 48}, 'A');
	//display.Refresh();
}
