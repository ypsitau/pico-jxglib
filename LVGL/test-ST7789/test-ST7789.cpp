#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

void Setup();

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	LVGL::Initialize();
	LVGL::Adapter lvglAdapter;
	lvglAdapter.EnableDoubleBuff(true).AttachOutput(display);
	lvglAdapter.AttachInput(UART::Default);
	Setup();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
	}
}

void Setup()
{
	lv_obj_t* buttonPrev = nullptr;
	for (int i = 0; i < 5; i++) {
		lv_obj_t* button = ::lv_button_create(::lv_screen_active());
		if (buttonPrev) {
			::lv_obj_align_to(button, buttonPrev, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
		} else {
			::lv_obj_align(button, LV_ALIGN_TOP_MID, 0, 4);
		}
		do {
			lv_obj_t* label = ::lv_label_create(button);
			::lv_label_set_text(label, "Button");
			::lv_obj_center(label);
		} while (0);
		buttonPrev = button;
	}
}
