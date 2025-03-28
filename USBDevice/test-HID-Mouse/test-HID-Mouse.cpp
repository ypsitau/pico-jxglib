#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice.h"

using namespace jxglib;

auto& GPIO_CURSOR_LEFT	= GPIO16;
auto& GPIO_CURSOR_UP	= GPIO17;
auto& GPIO_CURSOR_DOWN	= GPIO18;
auto& GPIO_CURSOR_RIGHT	= GPIO19;
auto& GPIO_LBUTTON		= GPIO20;
auto& GPIO_RBUTTON		= GPIO21;

//-----------------------------------------------------------------------------
// DeviceMouse
//-----------------------------------------------------------------------------
class DeviceMouse : public USBDevice::Mouse {
private:
	bool senseFlagPrev_;
public:
	DeviceMouse(USBDevice& device) : USBDevice::Mouse(device, "RaspberryPi Pico Mouse Interface", 0x81), senseFlagPrev_{false} {}
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
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HID Device (Mouse)", "0123456789ABCDEF",
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	DeviceMouse deviceMouse(device);
	device.Initialize();
	deviceMouse.Initialize();
	GPIO_CURSOR_LEFT	.init().set_dir_IN().pull_up();
	GPIO_CURSOR_UP		.init().set_dir_IN().pull_up();
	GPIO_CURSOR_DOWN	.init().set_dir_IN().pull_up();
	GPIO_CURSOR_RIGHT	.init().set_dir_IN().pull_up();
	GPIO_LBUTTON		.init().set_dir_IN().pull_up();
	GPIO_RBUTTON		.init().set_dir_IN().pull_up();
	for (;;) Tickable::Tick();
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
	if (!GPIO_CURSOR_LEFT.get())	{ senseFlag = true; x = -5; }
	if (!GPIO_CURSOR_UP.get())		{ senseFlag = true; y = -5; }
	if (!GPIO_CURSOR_DOWN.get())	{ senseFlag = true; y = 5; }
	if (!GPIO_CURSOR_RIGHT.get())	{ senseFlag = true; x = 5; }
	if (!GPIO_LBUTTON.get())		{ senseFlag = true; buttons |= 1 << 0; }
	if (!GPIO_RBUTTON.get())		{ senseFlag = true; buttons |= 1 << 1; }
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
