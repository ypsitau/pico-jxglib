#include <stdio.h>
#include <examples/lv_examples.h>
#include <demos/lv_demos.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/ILI9341.h"
#include "jxglib/LVGLAdapter.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"
#include "jxglib/KeyCode.h"

using namespace jxglib;

VT100::Decoder decoder;

void UARTHandler(void)
{
	UART& uart = UART::Default;
	while (uart.raw.is_readable()) decoder.FeedChar(uart.raw.getc());
}

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
	if (decoder.HasKeyData()) {
		int keyCode = decoder.GetKeyData();
		data->key =
			(keyCode == VK_TAB)?	LV_KEY_NEXT :
			//LV_KEY_PREV
			(keyCode == VK_RETURN)?	LV_KEY_ENTER :
			(keyCode == VK_UP)?		LV_KEY_UP :
			(keyCode == VK_DOWN)?	LV_KEY_DOWN :
			(keyCode == VK_LEFT)?	LV_KEY_LEFT :
			(keyCode == VK_RIGHT)?	LV_KEY_RIGHT :
			//LV_KEY_ESC
			(keyCode == VK_DELETE)?	LV_KEY_DEL :
			(keyCode == VK_BACK)?	LV_KEY_BACKSPACE :
			//LV_KEY_HOME
			//LV_KEY_END
			keyCode;
		data->state = LV_INDEV_STATE_PRESSED;
		::printf("Key: %02x\n", data->key);
	} else {
		data->state = LV_INDEV_STATE_RELEASED;
	}
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
	ILI9341::TouchScreen touchScreen(spi0, GPIO6, GPIO7);
	display.Initialize(Display::Dir::Rotate90);
	touchScreen.Initialize(display);
	LVGLAdapter lvglAdapter(false, 10);
	lvglAdapter.AttachOutput(display);
	lvglAdapter.AttachInput(touchScreen);
	UART::Default.irq_set_exclusive_handler(UARTHandler).irq_set_enabled(true);
	UART::Default.raw.set_irq_enables(true, false);
	//InputPointer inputPointer;
	InputKeypad inputKeypad;
	//InputButton inputButton;
	//InputEncoder inputEncoder;
	//lvglAdapter.SetInput_Pointer(inputPointer);
	do {
		lv_indev_t* indev = lvglAdapter.SetInput_Keypad(inputKeypad);
		lv_group_t* group = ::lv_group_create();
		::lv_group_set_default(group);
		::lv_indev_set_group(indev, group);
	} while (0);
	//lvglAdapter.SetInput_Button(inputButton);
	//lvglAdapter.SetInput_Encoder(inputEncoder);
	lvglAdapter.SetDefault();
	//::lv_example_textarea_1();
	//::lv_example_textarea_2();
	::lv_example_keyboard_1();
	for (;;) {
		::sleep_ms(5);
		::lv_timer_handler();
		::lv_tick_inc(5);
	}
}
