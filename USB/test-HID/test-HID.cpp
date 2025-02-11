#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBD.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public USBD::Keyboard {
private:
	int nKeycodePrev_;
public:
	Keyboard(USBD::Device& device) : USBD::Keyboard(device, "RaspberryPi Pico Keyboard", 0x81), nKeycodePrev_{0} {}
public:
	void Initialize();
	virtual void OnTask() override;
	virtual uint16_t On_GET_REPORT(uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength) override;
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) override;
	virtual void On_SET_REPORT(uint8_t reportID, hid_report_type_t report_type, const uint8_t* report, uint16_t reportLength) override;
	virtual void On_SET_PROTOCOL(uint8_t protocol) override;
};

//-----------------------------------------------------------------------------
// Mouse
//-----------------------------------------------------------------------------
class Mouse : public USBD::Mouse {
public:
	Mouse(USBD::Device& device) : USBD::Mouse(device, "RaspberryPi Pico Mouse", 0x82) {}
public:
	void Initialize();
	virtual void OnTask() override;
};

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBD::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBD::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
		iManufacturer:		0x01,
		iProduct:			0x02,
		iSerialNumber:		0x03,
	}, 0x0409, "RaspberryPi Pico HMI", "RaspberryPi Pico HMI Device", "0123456789ABCDEF");
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

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
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
	if (!hid_ready()) return;
	uint8_t report_id = 0;
	uint8_t modifier  = 0;
	uint8_t keycode[6] = { 0 };
	int nKeycode = 0;
	if (!GPIO16.get() && nKeycode < sizeof(keycode)) keycode[nKeycode++] = HID_KEY_ARROW_LEFT;
	if (!GPIO17.get() && nKeycode < sizeof(keycode)) keycode[nKeycode++] = HID_KEY_ARROW_UP;
	if (!GPIO18.get() && nKeycode < sizeof(keycode)) keycode[nKeycode++] = HID_KEY_ARROW_DOWN;
	if (!GPIO19.get() && nKeycode < sizeof(keycode)) keycode[nKeycode++] = HID_KEY_ARROW_RIGHT;
	if (!GPIO20.get() && nKeycode < sizeof(keycode)) keycode[nKeycode++] = HID_KEY_PAGE_UP;
	if (!GPIO21.get() && nKeycode < sizeof(keycode)) keycode[nKeycode++] = HID_KEY_PAGE_DOWN;
	if (::tud_suspended()) {
		// Wake up host if we are in suspend mode
		// and REMOTE_WAKEUP feature is enabled by host
		if (nKeycode > 0) ::tud_remote_wakeup();
		return;
	}
	if (nKeycode > 0) {
		hid_keyboard_report(report_id, modifier, keycode);
	} else if (nKeycodePrev_ > 0) {
		hid_keyboard_report(report_id, modifier, nullptr);
	}
	nKeycodePrev_ = nKeycode;
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

//-----------------------------------------------------------------------------
// Mouse
//-----------------------------------------------------------------------------
void Mouse::OnTask()
{
	if (!hid_ready()) return;
	bool btnB = !GPIO21.get();
	if (btnB) {
		uint8_t report_id = 0;
		uint8_t button_mask = 0;
		int8_t vertical = 0;
		int8_t horizontal = 0;
		int8_t delta = 5;
		hid_mouse_report(report_id, button_mask, delta, delta, vertical, horizontal);
	}
}
