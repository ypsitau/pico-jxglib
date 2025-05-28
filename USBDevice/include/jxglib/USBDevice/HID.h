//==============================================================================
// jxglib/USBDevice/HID.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_HID_H
#define PICO_JXGLIB_USBDEVICE_HID_H
#include "jxglib/USBDevice.h"

#if CFG_TUD_HID > 0

namespace jxglib::USBDevice {

class HID : public Interface {
protected:
	const uint8_t* reportDescSaved_;
public:
	HID(Controller& deviceController, uint32_t msecTick, const char* str, uint8_t protocol, const uint8_t* reportDesc, uint8_t bytesReportDesc,
							uint8_t endpInterrupt, uint8_t pollingInterval);
public:
	// Check if the interface is ready to use
	bool hid_ready() { return tud_hid_n_ready(iInstance_); }
	// Get interface supported protocol (bInterfaceProtocol) check out hid_interface_protocol_enum_t for possible values
	uint8_t hid_interface_protocol() { return tud_hid_n_interface_protocol(iInstance_); }
	// Get current active protocol: HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
	uint8_t hid_get_protocol() { return tud_hid_n_get_protocol(iInstance_); }
	// Send report to host
	bool hid_report(uint8_t reportId, void const* report, uint16_t len) { return tud_hid_n_report(iInstance_, reportId, report, len); }
	// KEYBOARD: convenient helper to send keyboard report if application
	// use template layout report as defined by hid_keyboard_report_t
	bool hid_keyboard_report(uint8_t reportId, uint8_t modifier, const uint8_t keycode[6]) { return tud_hid_n_keyboard_report(iInstance_, reportId, modifier, keycode); }
	// MOUSE: convenient helper to send mouse report if application
	// use template layout report as defined by hid_mouse_report_t
	bool hid_mouse_report(uint8_t reportId, uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal) { return tud_hid_n_mouse_report(iInstance_, reportId, buttons, x, y, vertical, horizontal); }
	// ABSOLUTE MOUSE: convenient helper to send absolute mouse report if application
	// use template layout report as defined by hid_abs_mouse_report_t
	bool hid_abs_mouse_report(uint8_t reportId, uint8_t buttons, int16_t x, int16_t y, int8_t vertical, int8_t horizontal) { return tud_hid_n_abs_mouse_report(iInstance_, reportId, buttons, x, y, vertical, horizontal); }
	// Gamepad: convenient helper to send gamepad report if application
	// use template layout report TUD_HID_REPORT_DESC_GAMEPAD
	bool hid_gamepad_report(uint8_t reportId, int8_t x, int8_t y, int8_t z, int8_t rz, int8_t rx, int8_t ry, uint8_t hat, uint32_t buttons) { return tud_hid_n_gamepad_report(iInstance_, reportId, x, y, z, rz, rx, ry, hat, buttons); }
public:
	const uint8_t* On_DESCRIPTOR_REPORT() { return reportDescSaved_; }
	virtual uint16_t On_GET_REPORT(uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength) { return 0; }
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) {}
	virtual void On_SET_REPORT(uint8_t reportID, hid_report_type_t reportType, const uint8_t* report, uint16_t reportLength) {}
	virtual void On_SET_PROTOCOL(uint8_t protocol) {}
};

class Keyboard : public HID {
public:
	struct KeyCodeToUsageId {
		uint8_t usageIdUS;
		uint8_t usageIdNonUS;
	};
private:
	static const uint8_t reportDesc_[];
public:
	static const KeyCodeToUsageId keyCodeToUsageIdTbl[];
public:
	Keyboard(Controller& deviceController, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
};


class Mouse : public HID {
private:
	static const uint8_t reportDesc_[];
public:
	Mouse(Controller& deviceController, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
};
class HIDCustom : public HID {
public:
	HIDCustom(Controller& deviceController, const char* str, const uint8_t* reportDesc,
					uint8_t bytesReportDesc, uint8_t endpInterrupt, uint8_t pollingInterval);
};

}

#endif
#endif
