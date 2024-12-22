#include <stdio.h>
#include <demos/lv_demos.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/LVGLAdapter.h"

using namespace jxglib;

void Setup();

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	::lv_init();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9341 display(spi1, 240, 320, GPIO11, GPIO10, GPIO12, GPIO13);
	ILI9341::TouchPanel touchPanel(spi0, GPIO6, GPIO7);
	display.Initialize(Display::Dir::Rotate90);
	touchPanel.Initialize(display);
	LVGLAdapter lvglAdapter(false, 10);
	lvglAdapter.AttachOutput(display);
	lvglAdapter.AttachInput(touchPanel);
	Setup();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}

void Setup()
{
}
