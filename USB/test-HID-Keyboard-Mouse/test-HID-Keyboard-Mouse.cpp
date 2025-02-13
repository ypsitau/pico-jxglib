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
	virtual void OnTask() override;
};

//-----------------------------------------------------------------------------
// Mouse
//-----------------------------------------------------------------------------
class Mouse : public USBD::Mouse {
public:
	Mouse(USBD::Device& device) : USBD::Mouse(device, "RaspberryPi Pico Mouse", 0x82) {}
public:
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
	GPIO16.init().set_dir_IN().pull_up();
	GPIO17.init().set_dir_IN().pull_up();
	GPIO18.init().set_dir_IN().pull_up();
	GPIO19.init().set_dir_IN().pull_up();
	GPIO20.init().set_dir_IN().pull_up();
	GPIO21.init().set_dir_IN().pull_up();
	for (;;) {
		device.Task();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
void Keyboard::OnTask()
{
	if (!hid_ready()) return;
	uint8_t report_id = 0;
	uint8_t modifier  = 0;
	uint8_t keycode[6] = { 0 };
	int nKeycode = 0;
	if (!GPIO16.get()) keycode[nKeycode++] = HID_KEY_ARROW_LEFT;
	if (!GPIO17.get()) keycode[nKeycode++] = HID_KEY_ARROW_UP;
	if (!GPIO18.get()) keycode[nKeycode++] = HID_KEY_ARROW_DOWN;
	if (!GPIO19.get()) keycode[nKeycode++] = HID_KEY_ARROW_RIGHT;
	if (::tud_suspended()) {
		// Wake up host if we are in suspend mode and REMOTE_WAKEUP feature is enabled by host
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

//-----------------------------------------------------------------------------
// Mouse
//-----------------------------------------------------------------------------
void Mouse::OnTask()
{
	if (!hid_ready()) return;
	if (!GPIO20.get()) {
		uint8_t report_id = 0;
		uint8_t button_mask = 0;
		int8_t x = -5, y = -5, vertical = 0, horizontal = 0;
		hid_mouse_report(report_id, button_mask, x, y, vertical, horizontal);
	}
	if (!GPIO21.get()) {
		uint8_t report_id = 0;
		uint8_t button_mask = 0;
		int8_t x = 5, y = 5, vertical = 0, horizontal = 0;
		hid_mouse_report(report_id, button_mask, x, y, vertical, horizontal);
	}
}
