#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/ST7789.h"
#include "jxglib/LVGLAdapter.h"

using namespace jxglib;

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
	ILI9341 display1(spi1, 240, 320, GPIO11, GPIO10, GPIO12, GPIO13);
	ST7789 display2(spi1, 240, 320, GPIO18, GPIO19, GPIO20, GPIO21);
	ILI9341::TouchScreen touchScreen(spi0, GPIO6, GPIO7);
	display1.Initialize(Display::Dir::Rotate90);
	display2.Initialize(Display::Dir::Rotate90);
	touchScreen.Initialize(display1);
	LVGLAdapter lvglAdapter1(false);
	LVGLAdapter lvglAdapter2(false);
	lvglAdapter1.AttachOutput(display1);
	lvglAdapter1.AttachInput(touchScreen);
	lvglAdapter2.AttachOutput(display2);
	lvglAdapter1.SetDefault();
	::lv_example_anim_3();
	lvglAdapter2.SetDefault();
	::lv_example_anim_2();
	for (;;) {
		::sleep_ms(1);
		::lv_timer_handler();
		::lv_tick_inc(1);
	}
}
