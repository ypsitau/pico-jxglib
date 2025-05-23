#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/SSD1306.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

void Setup();

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 400000);
	GPIO4.set_function_I2C0_SDA().pull_up();
	GPIO5.set_function_I2C0_SCL().pull_up();
	SSD1306 display(i2c0);
	display.Initialize();
	display.Clear().Refresh();
	LVGL::Initialize(5);
	LVGL::Adapter lvglAdapter;
	lvglAdapter.AttachDisplay(display);
	Setup();
	for (;;) Tickable::Tick();
}

void Setup()
{
	lv_obj_t* button = ::lv_button_create(::lv_screen_active());
	do {
		lv_obj_t* label = ::lv_label_create(button);
		::lv_label_set_text(label, "Button");
		::lv_obj_center(label);
	} while (0);
#if 0
	lv_obj_t* buttonPrev = nullptr;
	for (int i = 0; i < 1; i++) {
		lv_obj_t* button = ::lv_button_create(::lv_screen_active());
		if (buttonPrev) {
			::lv_obj_align_to(button, buttonPrev, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 4);
		} else {
			//::lv_obj_align(button, LV_ALIGN_TOP_MID, 0, 4);
			::lv_obj_center(button);
		}
		do {
			lv_obj_t* label = ::lv_label_create(button);
			::lv_label_set_text(label, "Button");
			::lv_obj_center(label);
		} while (0);
		buttonPrev = button;
	}
#endif
}
