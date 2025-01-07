#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/ILI9488.h"
#include "jxglib/LVGLAdapter.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);		// for touch screens
	::spi_init(spi1, 125 * 1000 * 1000);	// for displays
	::lv_init();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9488 display1(spi1, 320, 480, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	ILI9341 display2(spi1, 240, 320, {RST: GPIO19, DC: GPIO18, CS: GPIO20, BL: GPIO21});
	ILI9488::TouchScreen touchScreen1(spi0, {CS: GPIO6, IRQ: GPIO7});
	ILI9341::TouchScreen touchScreen2(spi0, {CS: GPIO8, IRQ: GPIO9});
	display1.Initialize(Display::Dir::Rotate270);
	display2.Initialize(Display::Dir::Rotate90);
	touchScreen1.Initialize(display1);
	touchScreen2.Initialize(display2);
	//-----------------------------------------
	// Attach display1 to LVGL
	LVGLAdapter lvglAdapter1(false);
	lvglAdapter1.AttachOutput(display1);
	lvglAdapter1.AttachInput(touchScreen1);
	::lv_example_anim_2();
	//::lv_example_textarea_2();
	//-----------------------------------------
	// Attach display2 to LVGL
	LVGLAdapter lvglAdapter2(false);
	lvglAdapter2.AttachOutput(display2);
	lvglAdapter2.AttachInput(UART::Default);	// shared by all the displays
	lvglAdapter2.AttachInput(touchScreen2);
	::lv_example_anim_3();
	//::lv_example_textarea_1();
	for (;;) {
		::sleep_ms(1);
		::lv_timer_handler();
		::lv_tick_inc(1);
	}
}
