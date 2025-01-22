#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "usb_descriptors.h"
#include "jxglib/GPIO.h"
#include "jxglib/USB.h"

void hid_task(void);

using namespace jxglib;

class Keyboard : public USB::Keyboard {
public:
	Keyboard(USB::Device& device) : USB::Keyboard(device, 0x81) {}
public:
	void Initialize();
	virtual void OnTask() override;
	virtual uint16_t On_GET_REPORT(uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength) override;
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) override;
	virtual void On_SET_REPORT(uint8_t reportID, hid_report_type_t report_type, const uint8_t* report, uint16_t reportLength) override;
	virtual void On_SET_PROTOCOL(uint8_t protocol) override;
};

class Mouse : public USB::Mouse {
public:
	Mouse(USB::Device& device) : USB::Mouse(device, 0x82) {}
public:
	void Initialize();
	virtual void OnTask() override;
};

int main(void)
{
	::stdio_init_all(); 
	USB::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USB::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
		iManufacturer:		0x01,
		iProduct:			0x02,
		iSerialNumber:		0x03,
	});
	Keyboard keyboard(device);
	Mouse mouse(device);
	device.Initialize();
	keyboard.Initialize();
	mouse.Initialize();
	for (;;) {
		device.Task();
	}
	return 0;
}


// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus

uint32_t board_millis()
{
	return ::to_ms_since_boot(::get_absolute_time());
}

void Keyboard::Initialize()
{
	GPIO16.init().set_dir_IN().pull_up();
	GPIO17.init().set_dir_IN().pull_up();
	GPIO18.init().set_dir_IN().pull_up();
	GPIO19.init().set_dir_IN().pull_up();
	GPIO20.init().set_dir_IN().pull_up();
}

void Keyboard::OnTask()
{
	const uint32_t interval_ms = 10;
	static uint32_t start_ms = 0;

	if (board_millis() - start_ms < interval_ms) return; // not enough time
	start_ms += interval_ms;

	if (!::tud_hid_n_ready(GetInterfaceNum())) return;

	//uint32_t const btn = board_button_read();
	bool btnLeft = !GPIO16.get();
	bool btnUp = !GPIO17.get();
	bool btnDown = !GPIO18.get();
	bool btnRight = !GPIO19.get();
	bool btnA = !GPIO20.get();
	//bool btnB = !GPIO21.get();
	// keyboard interface
	// used to avoid send multiple consecutive zero report for keyboard
	static bool has_keyboard_key = false;

	uint8_t report_id = 0;
	uint8_t modifier  = 0;

	if (btnLeft) {
		uint8_t keycode[6] = { 0 };
		keycode[0] = HID_KEY_ARROW_LEFT;
		::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
		has_keyboard_key = true;
	} else if (btnUp) {
		uint8_t keycode[6] = { 0 };
		keycode[0] = HID_KEY_ARROW_UP;
		::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
		has_keyboard_key = true;
	} else if (btnDown) {
		uint8_t keycode[6] = { 0 };
		keycode[0] = HID_KEY_ARROW_DOWN;
		::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
		has_keyboard_key = true;
	} else if (btnRight) {
		uint8_t keycode[6] = { 0 };
		keycode[0] = HID_KEY_ARROW_RIGHT;
		::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
		has_keyboard_key = true;
	} else if (btnA) {
		uint8_t keycode[6] = { 0 };
		keycode[0] = HID_KEY_PAGE_UP;
		::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
		has_keyboard_key = true;
#if 0
	} else if (btnB) {
		uint8_t keycode[6] = { 0 };
		keycode[0] = HID_KEY_PAGE_DOWN;
		::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
		has_keyboard_key = true;
#endif
	} else {
		// send empty key report if previously has key pressed
		if (has_keyboard_key) ::tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, NULL);
		has_keyboard_key = false;
	}
}

uint16_t Keyboard::On_GET_REPORT(uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength)
{
	::printf("On_GET_REPORT()\n");
	return 0;
}

void Keyboard::On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength)
{
	::printf("On_GET_REPORT_Complete()\n");
}

void Keyboard::On_SET_REPORT(uint8_t reportID, hid_report_type_t report_type, const uint8_t* report, uint16_t reportLength)
{
	::printf("On_SET_REPORT()\n");
	if (report_type == HID_REPORT_TYPE_OUTPUT) {
		if (reportLength < 1) {
			// do nothing
		} else if (report[0] & KEYBOARD_LED_CAPSLOCK) {
			//board_led_write(true);
		} else {
			//board_led_write(false);
		}
	}
}

void Keyboard::On_SET_PROTOCOL(uint8_t protocol)
{
	::printf("On_SET_PROTOCOL()\n");
}

void Mouse::Initialize()
{
	GPIO21.init().set_dir_IN().pull_up();
}

void Mouse::OnTask()
{
	if (!::tud_hid_n_ready(GetInterfaceNum())) return;
	bool btnB = !GPIO21.get();
	if (btnB) {
		uint8_t const report_id   = 0;
		uint8_t const button_mask = 0;
		int8_t  const vertical    = 0;
		int8_t  const horizontal  = 0;
		int8_t  const delta       = 5;
		::tud_hid_n_mouse_report(ITF_NUM_MOUSE, report_id, button_mask, delta, delta, vertical, horizontal);
	}
}
