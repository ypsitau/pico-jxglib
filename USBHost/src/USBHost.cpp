//==============================================================================
// USBHost.cpp
//==============================================================================
#include "jxglib/USBHost.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
USBHost::USBHost()
{
}

extern "C" {

void tuh_mount_cb(uint8_t devAddr)
{
	::printf("tuh_mount_cb(%d)\n", devAddr);
}

void tuh_umount_cb(uint8_t devAddr)
{
	::printf("tuh_umount_cb(%d)\n", devAddr);
}

#define MAX_REPORT  4

static struct
{
	uint8_t report_count;
	tuh_hid_report_info_t report_info[MAX_REPORT];
}hid_info[CFG_TUH_HID];

void tuh_hid_mount_cb(uint8_t devAddr, uint8_t instance, const uint8_t* descReport, uint16_t descLen)
{
	::printf("tuh_hid_mount_cb(%d, %d)\n", devAddr, instance);
	uint8_t itfProtocol = ::tuh_hid_interface_protocol(devAddr, instance);
	::printf("HID Interface Protocol = %s\r\n",
		(itfProtocol == HID_ITF_PROTOCOL_NONE)? "None" :
		(itfProtocol == HID_ITF_PROTOCOL_KEYBOARD)? "Keyboard" :
		(itfProtocol == HID_ITF_PROTOCOL_MOUSE)? "Mouse" : "unknown");
	// By default host stack will use activate boot protocol on supported interface.
	// Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
	if (itfProtocol == HID_ITF_PROTOCOL_NONE) {
		hid_info[instance].report_count = ::tuh_hid_parse_report_descriptor(
							hid_info[instance].report_info, MAX_REPORT, descReport, descLen);
		::printf("HID has %u reports \r\n", hid_info[instance].report_count);
	}
	// request to receive report
	// tuh_hid_report_received_cb() will be invoked when report is available
	if (!::tuh_hid_receive_report(devAddr, instance)) {
		::printf("Error: cannot request to receive report\r\n");
	}
}

void tuh_hid_umount_cb(uint8_t devAddr, uint8_t instance)
{
	::printf("tuh_hid_umount_cb(%d, %d)\n", devAddr, instance);
}

void tuh_hid_report_received_cb(uint8_t devAddr, uint8_t instance, const uint8_t* report, uint16_t len)
{
	uint8_t const itfProtocol = ::tuh_hid_interface_protocol(devAddr, instance);
	::printf("report\n");
	switch (itfProtocol) {
	case HID_ITF_PROTOCOL_NONE: {
		break;
	}
	case HID_ITF_PROTOCOL_KEYBOARD: {
		auto reportSpecific = *reinterpret_cast<const hid_keyboard_report_t*>(report);

		break;
	}
	case HID_ITF_PROTOCOL_MOUSE: {
		auto reportSpecific = *reinterpret_cast<const hid_mouse_report_t*>(report);
		break;
	}
	default: break;
	}
	if (!::tuh_hid_receive_report(devAddr, instance)) {
		printf("Error: cannot request to receive report\r\n");
	}
}

}

}
