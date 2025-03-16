#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice.h"

using namespace jxglib;

auto& GPIO_ARROW_LEFT	= GPIO16;
auto& GPIO_ARROW_UP		= GPIO17;
auto& GPIO_ARROW_DOWN	= GPIO18;
auto& GPIO_ARROW_RIGHT	= GPIO19;
auto& GPIO_PAGE_UP		= GPIO20;
auto& GPIO_PAGE_DOWN	= GPIO21;

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public USBDevice::Keyboard {
private:
	int nKeycodePrev_;
public:
	Keyboard(USBDevice::Device& device) : USBDevice::Keyboard(device, "RaspberryPi Pico Keyboard Interface", 0x81), nKeycodePrev_{0} {}
public:
	virtual void OnTick() override;
};

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBDevice::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HID Device (Keyboard)", "0123456789ABCDEF",
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	Keyboard keyboard(device);
	device.Initialize();
	keyboard.Initialize();
	GPIO_ARROW_LEFT		.init().set_dir_IN().pull_up();
	GPIO_ARROW_UP		.init().set_dir_IN().pull_up();
	GPIO_ARROW_DOWN		.init().set_dir_IN().pull_up();
	GPIO_ARROW_RIGHT	.init().set_dir_IN().pull_up();
	GPIO_PAGE_UP		.init().set_dir_IN().pull_up();
	GPIO_PAGE_DOWN		.init().set_dir_IN().pull_up();
	for (;;) Tickable::Tick();
}

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
void Keyboard::OnTick()
{
	uint8_t reportId = 0;
	uint8_t modifier  = 0;
	uint8_t keycode[6] = { 0 };
	int nKeycode = 0;
	if (!GPIO_ARROW_LEFT.get())		keycode[nKeycode++] = HID_KEY_ARROW_LEFT;
	if (!GPIO_ARROW_UP.get())		keycode[nKeycode++] = HID_KEY_ARROW_UP;
	if (!GPIO_ARROW_DOWN.get())		keycode[nKeycode++] = HID_KEY_ARROW_DOWN;
	if (!GPIO_ARROW_RIGHT.get())	keycode[nKeycode++] = HID_KEY_ARROW_RIGHT;
	if (!GPIO_PAGE_UP.get())		keycode[nKeycode++] = HID_KEY_PAGE_UP;
	if (!GPIO_PAGE_DOWN.get())		keycode[nKeycode++] = HID_KEY_PAGE_DOWN;
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
