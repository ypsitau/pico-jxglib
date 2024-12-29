#include <stdio.h>
#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
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
	ILI9341 display(spi1, 240, 320, {RST: GPIO11, DC: GPIO10, CS: GPIO12, BL: GPIO13});
	ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO6, IRQ: GPIO7});
	display.Initialize(Display::Dir::Rotate90);
	touchScreen.Initialize(display);
	LVGLAdapter lvglAdapter;
	lvglAdapter.AttachOutput(display);
	lvglAdapter.AttachInput(touchScreen);
	lvglAdapter.AttachInput(UART::Default);
	Setup();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}

void Setup()
{
	lv_obj_t* button = lv_button_create(lv_screen_active());
	//lv_obj_add_event_cb(button, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(button, LV_ALIGN_CENTER, 0, -40);
	lv_obj_set_width(button, 200);
	lv_obj_remove_flag(button, LV_OBJ_FLAG_PRESS_LOCK);
	do {
		lv_obj_t* label = lv_label_create(button);
		lv_label_set_text(label, "Button");
		lv_obj_center(label);
	} while (0);
}
