#include <stdio.h>
#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/LVGLAdapter.h"

using namespace jxglib;

class InputPointer : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputPointer::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Pointer::Handle\n");
	data->state = LV_INDEV_STATE_PRESSED;
	data->point.x = 0;
	data->point.y = 0;
}

class InputKeypad : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputKeypad::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Keypad::Handle\n");
	data->point.x = 0;
	data->point.y = 0;
	data->state = LV_INDEV_STATE_PRESSED;
	data->key = '1';
}

class InputButton : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputButton::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Button::Handle\n");
	data->state = LV_INDEV_STATE_PRESSED;
	data->btn_id = 0;
}

class InputEncoder : public LVGLAdapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputEncoder::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Encoder::Handle\n");
	data->state = LV_INDEV_STATE_PRESSED;	
	data->enc_diff = 0;
}

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
	ST7789 display1(spi1, 240, 320, GPIO18, GPIO19, GPIO20, GPIO21);
	ILI9341::TouchScreen touchScreen(spi0, GPIO6, GPIO7);
	display.Initialize(Display::Dir::Rotate90);
	display1.Initialize(Display::Dir::Rotate90);
	touchScreen.Initialize(display);
	LVGLAdapter lvglAdapter(false, 10);
	LVGLAdapter lvglAdapter1(false, 10);
	lvglAdapter.AttachOutput(display);
	lvglAdapter1.AttachOutput(display1);
	lvglAdapter.AttachInput(touchScreen);
	//InputPointer inputPointer;
	//InputKeypad inputKeypad;
	//InputButton inputButton;
	//InputEncoder inputEncoder;
	//lvglAdapter.SetInput_Pointer(inputPointer);
	//lvglAdapter.SetInput_Keypad(inputKeypad);
	//lvglAdapter.SetInput_Button(inputButton);
	//lvglAdapter.SetInput_Encoder(inputEncoder);
	lvglAdapter.SetDefault();
	::lv_example_textarea_1();
	lvglAdapter1.SetDefault();
	::lv_example_textarea_2();
	//do {
	//	lv_obj_t* button = lv_button_create(lv_screen_active());
	//	//lv_obj_add_event_cb(button, event_handler, LV_EVENT_ALL, NULL);
	//	lv_obj_align(button, LV_ALIGN_CENTER, 0, -40);
	//	lv_obj_set_width(button, 200);
	//	lv_obj_remove_flag(button, LV_OBJ_FLAG_PRESS_LOCK);
	//	do {
	//		lv_obj_t* label = lv_label_create(button);
	//		lv_label_set_text(label, "Button");
	//		lv_obj_center(label);
	//	} while (0);
	//} while (0);
	//do {
	//	lvglAdapter.SetDefault();
	//	lv_obj_t* btn1 = lv_button_create(lv_screen_active());
	//	//lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
	//	lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
	//	lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);
	//	lv_obj_t* label = lv_label_create(btn1);
	//	lv_label_set_text(label, "Button");
	//	lv_obj_center(label);
	//} while (0);
	//::printf("----1\n");
	//lv_refr_now(lvglAdapter.Get_lv_display());
	//::printf("----2\n");
	//lv_refr_now(lvglAdapter1.Get_lv_display());
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}
