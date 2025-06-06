#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice/HID.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// HID_Keyboard
//-----------------------------------------------------------------------------
class HID_Keyboard : public USBDevice::Keyboard {
private:
	int nKeycodePrev_;
	GPIO::KeyboardMatrix keyboard_;
public:
	HID_Keyboard(USBDevice::Controller& deviceController) : USBDevice::Keyboard(deviceController, "RaspberryPi Pico Keyboard Interface", 0x81), nKeycodePrev_{0} {}
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
	USBDevice::Controller deviceController({
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
	HID_Keyboard hidKeyboard(deviceController);
	deviceController.Initialize();
	hidKeyboard.Initialize();
	static const Keyboard::KeySet keySetTbl[] = {
		VK_1,    VK_2, VK_3,     VK_BACK,
		VK_4,    VK_5, VK_6,     VK_UP,
		VK_7,    VK_8, VK_9,     VK_DOWN,
		VK_LEFT, VK_0, VK_RIGHT, VK_RETURN, // {0, Keyboard::Mod::ShiftL},
	};
	const GPIO::KeyRow keyRowTbl[] = { GPIO16, GPIO17, GPIO18, GPIO19 };
	const GPIO::KeyCol keyColTbl[] = { GPIO20.pull_up(), GPIO21.pull_up(), GPIO26.pull_up(), GPIO27.pull_up() };
	hidKeyboard.GetKeyboard().Initialize(keySetTbl, keyRowTbl, count_of(keyRowTbl), keyColTbl, count_of(keyColTbl), GPIO::LogicNeg);
	for (;;) Tickable::Tick();
}

//-----------------------------------------------------------------------------
// HID_Keyboard
//-----------------------------------------------------------------------------
void HID_Keyboard::OnTick()
{
	uint8_t reportId = 0;
	uint8_t modifier  = keyboard_.GetModifier();
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
