#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice.h"

using namespace jxglib;

auto& GPIO_ARROW_LEFT			= GPIO16;
auto& GPIO_ARROW_UP				= GPIO17;
auto& GPIO_ARROW_DOWN			= GPIO18;
auto& GPIO_ARROW_RIGHT			= GPIO19;
auto& GPIO_CURSOR_UP_LEFT		= GPIO20;
auto& GPIO_CURSOR_DOWN_RIGHT	= GPIO21;

//-----------------------------------------------------------------------------
// HIDComposite
//-----------------------------------------------------------------------------
class HIDComposite : public USBDevice::HID {
public:
	struct ReportId {
		static const uint8_t Keyboard	= 0;
		static const uint8_t Mouse		= 1;
		static const uint8_t Max		= 2;
	};
private:
	int nKeycodePrev_;
	bool senseFlagPrev_;
private:
	static const uint8_t reportDesc_[];
public:
	HIDComposite(USBDevice::Device& device);
public:
	void SendReport(uint8_t reportId);
public:
	virtual void OnTick() override;
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) override;
};

const uint8_t HIDComposite::reportDesc_[] = {
	TUD_HID_REPORT_DESC_KEYBOARD(),
	//TUD_HID_REPORT_DESC_MOUSE()
	//TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(ReportId::Keyboard)),
	//TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(ReportId::Mouse))
};

HIDComposite::HIDComposite(USBDevice::Device& device) : USBDevice::HID(device, 30, "RaspberryPi Pico HID Composite Interface",
	HID_ITF_PROTOCOL_KEYBOARD, reportDesc_, sizeof(reportDesc_), 0x81, 5), nKeycodePrev_{0}, senseFlagPrev_{false} {}

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public USBDevice::HID {
public:
	struct ReportId {
		static const uint8_t Keyboard	= 1;
		static const uint8_t Mouse		= 2;
		static const uint8_t Max		= 3;
	};
private:
	int nKeycodePrev_;
private:
	static const uint8_t reportDesc_[];
public:
	Keyboard(USBDevice::Device& device);
public:
	virtual void OnTick() override;
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) override;
};

const uint8_t Keyboard::reportDesc_[] = {
	TUD_HID_REPORT_DESC_KEYBOARD()
	//TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(ReportId::Keyboard)),
	//TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(ReportId::Mouse))
};

Keyboard::Keyboard(USBDevice::Device& device) : USBDevice::HID(device, 30, "RaspberryPi Pico HID Composite Interface",
	HID_ITF_PROTOCOL_NONE, reportDesc_, sizeof(reportDesc_), 0x81, 5), nKeycodePrev_{0} {}

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
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HMI Composite Device (Keyboard + Mouse)", "0123456789ABCDEF",
		TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	HIDComposite hidComposite(device);
	//Keyboard keyboard(device);
	device.Initialize();
	hidComposite.Initialize();
	//keyboard.Initialize();
	GPIO_ARROW_LEFT			.init().set_dir_IN().pull_up();
	GPIO_ARROW_UP			.init().set_dir_IN().pull_up();
	GPIO_ARROW_DOWN			.init().set_dir_IN().pull_up();
	GPIO_ARROW_RIGHT		.init().set_dir_IN().pull_up();
	GPIO_CURSOR_UP_LEFT		.init().set_dir_IN().pull_up();
	GPIO_CURSOR_DOWN_RIGHT	.init().set_dir_IN().pull_up();
	for (;;) Tickable::Tick();
}

//-----------------------------------------------------------------------------
// HIDComposite
//-----------------------------------------------------------------------------
void HIDComposite::SendReport(uint8_t reportId)
{
	switch (reportId) {
	case ReportId::Keyboard: {
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
		break;
	}
	case ReportId::Mouse: {
		bool senseFlag = false;
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
		break;
	}
	default: break;
	}
}

void HIDComposite::OnTick()
{
	SendReport(ReportId::Keyboard);
}

void HIDComposite::On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength)
{
	::printf("On_GET_REPORT_Complete\n");
	uint8_t reportIdNext = report[0] + 1;
	if (reportIdNext < ReportId::Max) SendReport(reportIdNext);
}

//-----------------------------------------------------------------------------
// Keyboard
//-----------------------------------------------------------------------------
void Keyboard::OnTick()
{
	uint8_t reportId = 0; //ReportId::Keyboard;
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

void Keyboard::On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength)
{
	::printf("On_GET_REPORT_Complete\n");
}
