#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Terminal.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	display.Initialize();
	Terminal terminal;
	terminal.AttachOutput(display);
	terminal.SetFont(Font::shinonome16);
	for (int i = 0; i < 1000; i++) {
		terminal.printf("hoge %d\n", i);
	}
}
