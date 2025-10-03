#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Display/ILI9488.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);		// for touch screens
	::spi_init(spi1, 125'000'000);	// for displays
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
	display.Initialize(Display::Dir::Rotate0);
	touchScreen.Initialize(display);
	LVGL::Initialize(5);
	//-----------------------------------------
	// Create an adapter and attach it to the display
	LVGL::Adapter lvglAdapter1;
	lvglAdapter1.EnableDoubleBuff(false)
			.AttachDisplay(display, {0, 0, 240, 100}).AttachTouchScreen(touchScreen);
	::lv_example_style_13();
	//-----------------------------------------
	// Create another adapter and attach it to the display
	LVGL::Adapter lvglAdapter2;
	lvglAdapter2.EnableDoubleBuff(false)
			.AttachDisplay(display, {0, 100, 240, 220}).AttachTouchScreen(touchScreen);
	::lv_example_textarea_1();
	for (;;) Tickable::Tick();
}
