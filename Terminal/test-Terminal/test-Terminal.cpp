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
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	Terminal terminal;
	terminal.Initialize(800);
	terminal.AttachOutput(display);
	terminal.SetColorBg(Color::blue).ClearScreen();
	terminal.SetFont(Font::shinonome16);
	terminal.SetSpacingRatio(1., 1.);
	int i = 0;
	for ( ; i < 8; i++) {
		terminal.Printf("%d: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n", i);
	}
	for (;;) {
		Printf("[D]Dump [H]History [A]Add Line\n");
		char ch = getchar();
		if (ch == 'd') {
			terminal.GetLineBuff().Print();
		} else if (ch == 'h') {
			UART::Default.PrintFrom(terminal.CreateStream());
		} else if (ch == 'a') {
			terminal.Printf("%d: ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n", i);
			i++;
		}
	}
}
