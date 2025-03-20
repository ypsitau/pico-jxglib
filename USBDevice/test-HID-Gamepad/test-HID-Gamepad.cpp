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
auto& GPIO_BUTTON_A		= GPIO20;
auto& GPIO_BUTTON_B		= GPIO21;

//-----------------------------------------------------------------------------
// Gamepad
//-----------------------------------------------------------------------------
class Gamepad : public USBDevice::Gamepad {
public:
	Gamepad(USBDevice& device) : USBDevice::Gamepad(device, "RaspberryPi Pico Gamepad Interface", 0x81) {}
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
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HID Device (Gamepad)", "0123456789ABCDEF",
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	Gamepad gamepad(device);
	device.Initialize();
	gamepad.Initialize();
	GPIO_ARROW_LEFT		.init().set_dir_IN().pull_up();
	GPIO_ARROW_UP		.init().set_dir_IN().pull_up();
	GPIO_ARROW_DOWN		.init().set_dir_IN().pull_up();
	GPIO_ARROW_RIGHT	.init().set_dir_IN().pull_up();
	GPIO_BUTTON_A		.init().set_dir_IN().pull_up();
	GPIO_BUTTON_B		.init().set_dir_IN().pull_up();
	for (;;) Tickable::Tick();
}

//-----------------------------------------------------------------------------
// Gamepad
//-----------------------------------------------------------------------------
void Gamepad::OnTick()
{
	uint8_t reportId = 0;
	int8_t x = 0, y = 0, z = 0, rz = 0, rx = 0, ry = 0, hat = 0;
	uint32_t buttons = 0;
	if (!GPIO_ARROW_LEFT.get()) x = -4;
	if (!GPIO_ARROW_RIGHT.get()) x = +4;
	if (!GPIO_BUTTON_A.get()) buttons |= (1 << 0);
	if (!GPIO_BUTTON_B.get()) buttons |= (1 << 1);
	::printf("chek %04x\n", buttons);
	if (hid_ready()) {
		hid_gamepad_report(reportId, x, y, z, rz, rx, ry, hat, buttons);
	}
}
