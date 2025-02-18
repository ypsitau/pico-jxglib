#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/ILI9488.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 2 * 1000 * 1000);		// for touch screens
	::spi_init(spi1, 125 * 1000 * 1000);	// for displays
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
	display.Initialize(Display::Dir::Rotate0);
	touchScreen.Initialize(display);
	LVGL::Initialize();
	//-----------------------------------------
	// Create another adapter and attach it to the display
	LVGL::Adapter lvglAdapter;
	lvglAdapter.AttachOutput(display, {0, 220, 240, 100});
	lvglAdapter.AttachInput(touchScreen);
	do {
		static const char *labelTbl[] = {
			"RollUp", "Dump", "\n",
			"RollDown", "Print Buffer", "",
		};
		lv_obj_t* btnm = lv_buttonmatrix_create(lv_screen_active());
		lv_obj_set_size(btnm, 230, 90);
		lv_obj_align(btnm, LV_ALIGN_BOTTOM_MID, 0, -5);
		//lv_obj_add_event_cb(btnm, btnm_event_handler, LV_EVENT_VALUE_CHANGED, ta);
		lv_obj_remove_flag(btnm, LV_OBJ_FLAG_CLICK_FOCUSABLE); /*To keep the text area focused on button clicks*/
		lv_buttonmatrix_set_map(btnm, labelTbl);
	
	} while (0);
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
	}
}
