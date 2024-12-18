#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ILI9341.h"
#include "jxglib/AdapterLVGL.h"
#include "jxglib/Drawable_TestCase.h"

using namespace jxglib;

void lv_example_button_1();
static void event_handler(lv_event_t * e);

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO::clr_mask(0xffffffff);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	//ST7789 display(spi0, 240, 240, GPIO6, GPIO7, GPIO::None);
	//ST7789 display(spi1, 240, 320, GPIO8, GPIO9, GPIO10, GPIO::None);
	ILI9341 display(spi1, 240, 320, GPIO12, GPIO11, GPIO13, GPIO::None);
	display.Initialize(Display::Dir::Rotate0);
	//Drawable* drawableTbl[] = { &display };
	//Drawable_TestCase::DrawString(drawableTbl, count_of(drawableTbl));
	AdapterLVGL::AttachOutput(display);
	
	lv_indev_t *indev = ::lv_indev_create();
	::lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
	//lv_indev_set_read_cb(indev, xpt2046_read_cb);

	lv_example_button_1();

	while (1)
	{
		sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}

static void event_handler(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
}

void lv_example_button_1(void)
{
	lv_obj_t* screen = AdapterLVGL::GetScreen();
	lv_obj_t* label;

	lv_obj_t* btn1 = lv_button_create(screen);
	lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
	lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
	label = lv_label_create(btn1);
	lv_label_set_text(label, "Button");
	lv_obj_center(label);
}
