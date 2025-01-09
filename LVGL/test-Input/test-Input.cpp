#include <stdio.h>
#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/LVGL.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"
#include "jxglib/KeyCode.h"

using namespace jxglib;

VT100::Decoder decoder;

class InputPointer : public LVGL::Adapter::Input {
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

class InputKeypad : public LVGL::Adapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputKeypad::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	data->key = LV_KEY_ENTER;
	data->state = LV_INDEV_STATE_PRESSED;
}

class InputButton : public LVGL::Adapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

void InputButton::Handle(lv_indev_t* indev_drv, lv_indev_data_t* data)
{
	//::printf("Button::Handle\n");
	data->state = LV_INDEV_STATE_PRESSED;
	data->btn_id = 0;
}

class InputEncoder : public LVGL::Adapter::Input {
public:
	virtual void Handle(lv_indev_t* indev_drv, lv_indev_data_t* data) override;
};

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
	LVGL::Adapter lvglAdapter(false, 10);
	lvglAdapter.AttachOutput(display);
	lvglAdapter.AttachInput(touchScreen);
	lvglAdapter.AttachInput(UART::Default);
	//InputPointer inputPointer;
	//InputKeypad inputKeypad;
	//InputButton inputButton;
	//InputEncoder inputEncoder;
	//lvglAdapter.SetInput_Pointer(inputPointer);
	//lvglAdapter.SetInput_Keypad(inputKepad);
	//lvglAdapter.SetInput_Button(inputButton);
	//lvglAdapter.SetInput_Encoder(inputEncoder);
	lvglAdapter.SetDefault();
	//::lv_example_textarea_1();
	::lv_example_textarea_2();
	//::lv_example_keyboard_1();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}
