#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/ST7789.h"

using namespace jxglib;

int main()
{
	Canvas canvas;
	stdio_init_all();
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, GPIO10, GPIO11, GPIO12, GPIO13);
	display.Initialize();
	canvas.AttachOutput(display);
	//isplay.DrawRectFill(0, 0, 100, 100);
	canvas.Fill(Color::red);
	int x = 0, y = 0;
	canvas.Clear();
	canvas.DrawRectFill(x, y, 100, 100, Color::white);
	canvas.Refresh();

}
