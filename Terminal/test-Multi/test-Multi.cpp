#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome12.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	Terminal terminal1, terminal2, terminal3;
	terminal1.Initialize();
	terminal2.Initialize();
	terminal3.Initialize();
	terminal1.AttachOutput(display, {0, 0, 240, 160});
	terminal2.AttachOutput(display, {0, 160, 120, 160});
	terminal3.AttachOutput(display, {120, 160, 120, 160});
	terminal1.SetColorBg(Color(0, 0, 32)).ClearScreen();
	terminal2.SetColorBg(Color(0, 32, 0)).ClearScreen();
	terminal3.SetColorBg(Color(32, 0, 0)).ClearScreen();
	terminal1.SetFont(Font::shinonome16);
	terminal2.SetFont(Font::shinonome12);
	terminal3.SetFont(Font::shinonome12);
	terminal1.Printf("[Terminal1]\nABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n");
	terminal2.Printf("[Terminal2]\nABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n");
	terminal3.Printf("[Terminal3]\nABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n");
}
