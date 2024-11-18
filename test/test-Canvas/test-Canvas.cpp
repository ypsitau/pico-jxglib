#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Canvas.h"
#include "jxglib/ST7789.h"
#include "image/image_cat_240x320.h"

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
	int x = 0, y = 0;
	//canvas.Clear();
	for (;;) {
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate0);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate90);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate180);
		canvas.Refresh();
		canvas.DrawImage(0, 0, image_cat_240x320, nullptr, Canvas::ImageDir::Rotate270);
		canvas.Refresh();
	}
}
