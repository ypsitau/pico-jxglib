#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"
#include "jxglib/SSD1306.h"
#include "jxglib/ST7735.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/Font/shinonome14.h"
#include "jxglib/Font/shinonome16.h"
#include "jxglib/Font/sisd24x32.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	//::i2c_init(i2c0, 400000);
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	//GPIO4.set_function_I2C0_SDA().pull_up();
	//GPIO5.set_function_I2C0_SCL().pull_up();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	//SSD1306 display0(i2c0);
	ST7789 display1(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display1.Initialize(Display::Dir::Rotate0);
	//display1.SetFont(Font::shinonome16);
	//display1.DrawString(0, 0, "Hello");
	//for (;;) ;
	//display0.Clear().Refresh();
	//Terminal terminal0;
	Terminal terminal1;
	terminal1.Initialize(800);
	//terminal0.Dump.DigitsAddr_Auto().Cols(6);
	uint32_t addr = 0x10000000;
	terminal1.Dump.DigitsAddr_Auto().AddrStart(addr).Cols(8);
	//terminal0.AttachOutput(display0);
	terminal1.AttachOutput(display1, Terminal::AttachDir::Rotate0);
	//terminal1.AttachOutput(display1, {0, 0, 240, 100}, Terminal::AttachDir::Rotate0);
	//terminal1.AttachOutput(display1, {50, 120, 240, 100}, Terminal::AttachDir::Rotate0);
	//terminal0.SetFont(Font::shinonome12);
	//terminal1.SetFont(Font::shinonome14);
	terminal1.SetColorBg(Color::blue);
	terminal1.SetFont(Font::shinonome16);
	terminal1.SetSpacingRatio(1., 1.);
	//terminal0.Dump(reinterpret_cast<const void*>(0), 8 * 20);
	//terminal1.Dump(reinterpret_cast<const void*>(addr), 8 * 100);
	int i = 0;
	for ( ; i < 8; i++) {
		terminal1.Printf("%d: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n", i);
	}
	for (;;) {
		Printf("press any key\n");
		char ch = getchar();
		if (ch == 'd') {
			terminal1.GetLineBuff().Print();
		} else if (ch == 'p') {
			UART::Default.PrintFrom(terminal1.CreateStream());
		} else {
			terminal1.Printf("%d: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n", i);
			i++;
		}
	}
#if 0
	Terminal terminal;
	terminal.AttachOutput(display2, Terminal::AttachDir::Rotate0);
	//terminal.AttachOutput(display2, Terminal::AttachDir::Rotate90);
	//terminal.AttachOutput(display3);
	//terminal.AttachOutput(display4, Terminal::AttachDir::Rotate90);
	terminal.SetFont(Font::shinonome12);
	//terminal.SetFont(Font::shinonome16);
	//terminal.SetFont(Font::sisd24x32);
	//terminal.SetColorBg(Color::green);
	terminal.Clear();
	//terminal.SetFont(Font::shinonome16);
	terminal.DumpStyle.DigitsAddr(4).Cols((terminal.GetColNum() - 4 - 1) / 3);
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
