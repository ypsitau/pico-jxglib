#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// DeviceKeyboard
//-----------------------------------------------------------------------------
class DeviceKeyboard : public USBDevice::Keyboard {
private:
	int nKeycodePrev_;
	GPIO::KeyboardMatrix keyboard_;
public:
	DeviceKeyboard(USBDevice& device) : USBDevice::Keyboard(device, "RaspberryPi Pico Keyboard Interface", 0x81), nKeycodePrev_{0} {}
public:
	GPIO::KeyboardMatrix& GetKeyboard() { return keyboard_; }
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
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HID Device (Keyboard)", "0123456789ABCDEF",
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	DeviceKeyboard deviceKeyboard(device);
	device.Initialize();
	deviceKeyboard.Initialize();
	static const uint8_t keyCodeTbl[] = {
		VK_1, VK_2, VK_3, VK_BACK,
		VK_4, VK_5, VK_6, VK_LEFT,
		VK_7, VK_8, VK_9, VK_RIGHT,
		VK_OEM_COMMA, VK_0, VK_OEM_PERIOD, VK_RETURN,
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
	deviceKeyboard.GetKeyboard().Initialize(keyCodeTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
	for (;;) Tickable::Tick();
}

//-----------------------------------------------------------------------------
// DeviceKeyboard
//-----------------------------------------------------------------------------
void DeviceKeyboard::OnTick()
{
	uint8_t reportId = 0;
	uint8_t modifier  = 0;
	uint8_t usageIdTbl[6] = { 0 };
	uint8_t keyCodeTbl[6];
	int nKeyCode = keyboard_.SenseKeyCode(keyCodeTbl, count_of(keyCodeTbl));
	int nUsageId = 0;
	for (int i = 0; i < nKeyCode; i++) {
		uint8_t usageId = keyCodeToUsageIdTbl[keyCodeTbl[i]].usageIdUS;
		if (usageId) usageIdTbl[nUsageId++] = usageId;
	}
	if (::tud_suspended()) {
		// Wake up host if we are in suspend mode and REMOTE_WAKEUP feature is enabled by host
		if (nUsageId > 0) ::tud_remote_wakeup();
	} else if (!hid_ready()) {
		// do nothing
	} else if (nUsageId > 0) {
		hid_keyboard_report(reportId, modifier, usageIdTbl);
	} else if (nKeycodePrev_ > 0) {
		hid_keyboard_report(reportId, modifier, nullptr);
	}
	nKeycodePrev_ = nUsageId;
}
