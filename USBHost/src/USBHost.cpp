//==============================================================================
// USBHost.cpp
//==============================================================================
#include "jxglib/USBHost.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
USBHost* USBHost::Instance = nullptr;

USBHost::USBHost()
{
	Instance = this;
}

void USBHost::Initialize(uint8_t rhport)
{
	::tuh_init(rhport);
}

void USBHost::OnTick()
{
	::tuh_task();
}

extern "C" {

void tuh_mount_cb(uint8_t devAddr)
{
	USBHost::Instance->OnMount(devAddr);
}

void tuh_umount_cb(uint8_t devAddr)
{
	USBHost::Instance->OnUmount(devAddr);
}

#define MAX_REPORT  4

static struct {
	uint8_t report_count;
	tuh_hid_report_info_t report_info[MAX_REPORT];
} hid_info[CFG_TUH_HID];

void tuh_hid_mount_cb(uint8_t devAddr, uint8_t iInstance, const uint8_t* descReport, uint16_t descLen)
{
	::printf("tuh_hid_mount_cb(%d, %d)\n", devAddr, iInstance);
	uint8_t itfProtocol = ::tuh_hid_interface_protocol(devAddr, iInstance);
	::printf("HID Interface Protocol = %s\r\n",
		(itfProtocol == HID_ITF_PROTOCOL_NONE)? "None" :
		(itfProtocol == HID_ITF_PROTOCOL_KEYBOARD)? "Keyboard" :
		(itfProtocol == HID_ITF_PROTOCOL_MOUSE)? "Mouse" : "unknown");
	// By default host stack will use activate boot protocol on supported interface.
	// Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
	if (itfProtocol == HID_ITF_PROTOCOL_NONE) {
		hid_info[iInstance].report_count = ::tuh_hid_parse_report_descriptor(
							hid_info[iInstance].report_info, MAX_REPORT, descReport, descLen);
		::printf("HID has %u reports \r\n", hid_info[iInstance].report_count);
	}
	// request to receive report
	// tuh_hid_report_received_cb() will be invoked when report is available
	if (!::tuh_hid_receive_report(devAddr, iInstance)) {
		::printf("Error: cannot request to receive report\r\n");
	}
}

void tuh_hid_umount_cb(uint8_t devAddr, uint8_t iInstance)
{
	::printf("tuh_hid_umount_cb(%d, %d)\n", devAddr, iInstance);
}

void tuh_hid_report_received_cb(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len)
{
	uint8_t const itfProtocol = ::tuh_hid_interface_protocol(devAddr, iInstance);
	switch (itfProtocol) {
	case HID_ITF_PROTOCOL_NONE: {
		break;
	}
	case HID_ITF_PROTOCOL_KEYBOARD: {
		USBHost::GetKeyboard().OnReport(devAddr, iInstance, *reinterpret_cast<const hid_keyboard_report_t*>(report));
		break;
	}
	case HID_ITF_PROTOCOL_MOUSE: {
		USBHost::GetMouse().OnReport(devAddr, iInstance, *reinterpret_cast<const hid_mouse_report_t*>(report));
		break;
	}
	default: break;
	}
	if (!::tuh_hid_receive_report(devAddr, iInstance)) {
		printf("Error: cannot request to receive report\r\n");
	}
}

}

//------------------------------------------------------------------------------
// USBHost::Keyboard
//------------------------------------------------------------------------------
const uint8_t USBHost::Keyboard::convTbl_101Keyboard[128][2] = { HID_KEYCODE_TO_ASCII };

USBHost::Keyboard::Keyboard() : Tickable(100), modifier_{0}, cntHold_{0}
{
	::memset(keycode_, 0x00, sizeof(keycode_));
}

void USBHost::Keyboard::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report)
{
	modifier_ = report.modifier;
	::memcpy(keycode_, report.keycode, sizeof(keycode_));
	if (keycode_[0]) {
		fifoKeyData_.WriteData(CreateKeyData(keycode_[0], modifier_));
		cntHold_ = 5;
	}
}

bool USBHost::Keyboard::GetKeyData(KeyData& keyData)
{
	if (fifoKeyData_.HasData()) {
		keyData = fifoKeyData_.ReadData();
		return true;
	}
	return false;
}

void USBHost::Keyboard::OnTick()
{
	if (cntHold_ > 0) {
		cntHold_--;
		return;
	}
	if (keycode_[0]) {
		fifoKeyData_.WriteData(CreateKeyData(keycode_[0], modifier_));
	}
}

KeyData USBHost::Keyboard::CreateKeyData(uint8_t keycode, uint8_t modifier)
{
	bool isShiftDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT));
	char charCode = (keycode >= 128)? 0 : convTbl_101Keyboard[keycode][isShiftDown? 1 : 0];
	return (charCode == 0)? KeyData(keycode, true, modifier) : KeyData(charCode, false, modifier);
}

//------------------------------------------------------------------------------
// USBHost::Mouse
//------------------------------------------------------------------------------
USBHost::Mouse::Mouse()
{
}

void USBHost::Mouse::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report)
{
	report.buttons;
	report.pan;
	report.wheel;
	report.x;
	report.y;
}

}
