#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice/HID.h"

using namespace jxglib;

auto& GPIO_ARROW_LEFT			= GPIO16;
auto& GPIO_ARROW_UP				= GPIO17;
auto& GPIO_ARROW_DOWN			= GPIO18;
auto& GPIO_ARROW_RIGHT			= GPIO19;
auto& GPIO_CURSOR_UP_LEFT		= GPIO20;
auto& GPIO_CURSOR_DOWN_RIGHT	= GPIO21;

//-----------------------------------------------------------------------------
// DeviceKeyboard
//-----------------------------------------------------------------------------
class DeviceKeyboard : public USBDevice::Keyboard {
private:
	int nKeycodePrev_;
public:
	DeviceKeyboard(USBDevice::Controller& device) : USBDevice::Keyboard(device, "RaspberryPi Pico Keyboard Interface", 0x81), nKeycodePrev_{0} {}
public:
	virtual void OnTick() override;
};

//-----------------------------------------------------------------------------
// DeviceMouse
//-----------------------------------------------------------------------------
class DeviceMouse : public USBDevice::Mouse {
private:
	bool senseFlagPrev_;
public:
	DeviceMouse(USBDevice::Controller& device) : USBDevice::Mouse(device, "RaspberryPi Pico Mouse Interface", 0x82), senseFlagPrev_{false} {}
public:
	virtual void OnTick() override;
};

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBDevice device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HMI Device (Keyboard + Mouse)", "0123456789ABCDEF",
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	DeviceKeyboard deviceKeyboard(device);
	DeviceMouse deviceMouse(device);
	device.Initialize();
	deviceKeyboard.Initialize();
	deviceMouse.Initialize();
	GPIO_ARROW_LEFT			.init().set_dir_IN().pull_up();
	GPIO_ARROW_UP			.init().set_dir_IN().pull_up();
	GPIO_ARROW_DOWN			.init().set_dir_IN().pull_up();
	GPIO_ARROW_RIGHT		.init().set_dir_IN().pull_up();
	GPIO_CURSOR_UP_LEFT		.init().set_dir_IN().pull_up();
	GPIO_CURSOR_DOWN_RIGHT	.init().set_dir_IN().pull_up();
	for (;;) Tickable::Tick();
}

//-----------------------------------------------------------------------------
// DeviceKeyboard
//-----------------------------------------------------------------------------
void DeviceKeyboard::OnTick()
{
	uint8_t reportId = 0;
	uint8_t modifier  = 0;
	uint8_t keycode[6] = { 0 };
	int nKeycode = 0;
	if (!GPIO_ARROW_LEFT.get())		keycode[nKeycode++] = HID_KEY_ARROW_LEFT;
	if (!GPIO_ARROW_UP.get())		keycode[nKeycode++] = HID_KEY_ARROW_UP;
	if (!GPIO_ARROW_DOWN.get())		keycode[nKeycode++] = HID_KEY_ARROW_DOWN;
	if (!GPIO_ARROW_RIGHT.get())	keycode[nKeycode++] = HID_KEY_ARROW_RIGHT;
	if (::tud_suspended()) {
		// Wake up host if we are in suspend mode and REMOTE_WAKEUP feature is enabled by host
		if (nKeycode > 0) ::tud_remote_wakeup();
	} else if (!hid_ready()) {
		// do nothing
	} else if (nKeycode > 0) {
		hid_keyboard_report(reportId, modifier, keycode);
	} else if (nKeycodePrev_ > 0) {
		hid_keyboard_report(reportId, modifier, nullptr);
	}
	nKeycodePrev_ = nKeycode;
}

//-----------------------------------------------------------------------------
// DeviceMouse
//-----------------------------------------------------------------------------
void DeviceMouse::OnTick()
{
	bool senseFlag = false;
	uint8_t reportId = 0;
	uint8_t buttons = 0;
	int8_t x = 0, y = 0;
	int8_t vertical = 0, horizontal = 0;
	if (!GPIO_CURSOR_UP_LEFT.get())		{ senseFlag = true; x = y = -5; }
	if (!GPIO_CURSOR_DOWN_RIGHT.get())	{ senseFlag = true; x = y = 5; }
	if (::tud_suspended()) {
		// Wake up host if we are in suspend mode and REMOTE_WAKEUP feature is enabled by host
		if (senseFlag) ::tud_remote_wakeup();
	} else if (!hid_ready()) {
		// do nothing
	} else if (senseFlag || senseFlagPrev_) {
		hid_mouse_report(reportId, buttons, x, y, vertical, horizontal);
	}
	senseFlagPrev_ = senseFlag;
}
