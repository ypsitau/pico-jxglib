#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice/HID.h"
#include "jxglib/PackedNumber.h"

using namespace jxglib;

auto& GPIO_ARROW_LEFT	= GPIO16;
auto& GPIO_ARROW_UP		= GPIO17;
auto& GPIO_ARROW_DOWN	= GPIO18;
auto& GPIO_ARROW_RIGHT	= GPIO19;
auto& GPIO_BUTTON_A		= GPIO20;
auto& GPIO_BUTTON_B		= GPIO21;

//-----------------------------------------------------------------------------
// HID_Gamepad
// https://www.usb.org/document-library/hid-descriptor-tool
//-----------------------------------------------------------------------------
class HID_Gamepad : public USBDevice::HIDCustom {
public:
	struct Report {
		uint8_t buttons;
		int16_t rx;
		int16_t ry;
	} __attribute__((__packed__));
private:
	static const uint8_t reportDesc_[];
public:
	HID_Gamepad(USBDevice::Controller& deviceController);
public:
	virtual void OnTick() override;
};

const uint8_t HID_Gamepad::reportDesc_[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
	//
	0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x02,                    //     USAGE_MAXIMUM (Button 2)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	//
    0x75, 0x06,                    //     REPORT_SIZE (6)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	//
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x33,                    //     USAGE (rX)
    0x09, 0x34,                    //     USAGE (rY)
    0x09, 0x35,                    //     USAGE (rZ)
	0x09, 0x36,
	0x09, 0x37,
	0x09, 0x38,
	0x09, 0x39,
	0x09, 0x3a,
	0x09, 0x3b,
	0x09, 0x3c,
	0x09, 0x3d,
	0x09, 0x3e,
	0x09, 0x3f,

	0x16, 0x01, 0x80,              //     LOGICAL_MINIMUM (-32767)
    0x26, 0xff, 0x7f,              //     LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                    //     REPORT_SIZE (16) ... must be 16 bit for Windows
	0x95, 0x10,                    //     REPORT_COUNT (16)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
	//
	0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION	
};

HID_Gamepad::HID_Gamepad(USBDevice::Controller& deviceController) : USBDevice::HIDCustom(deviceController, "RaspberryPi Pico Gamepad Interface",
							reportDesc_, sizeof(reportDesc_), 0x81, 10) {}

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
	}, 0x0409, "pico-jxglib sample", "RaspberryPi Pico HID Device (Gamepad)", "0123456789ABCDEF",
		0); //TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);
	HID_Gamepad gamepad(deviceController);
	deviceController.Initialize();
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
// HID_Gamepad
//-----------------------------------------------------------------------------
void HID_Gamepad::OnTick()
{
	uint8_t reportId = 0;
	Report report;
	::memset(&report, 0x00, sizeof(report));
	if (!GPIO_ARROW_LEFT.get()) report.rx = -1;
	if (!GPIO_ARROW_RIGHT.get()) report.rx = +1;
	if (!GPIO_ARROW_UP.get()) report.ry = -1;
	if (!GPIO_ARROW_DOWN.get()) report.ry = +1;
	if (!GPIO_BUTTON_A.get()) report.buttons |= (1 << 0);
	if (!GPIO_BUTTON_B.get()) report.buttons |= (1 << 1);
	if (hid_ready()) {
		hid_report(reportId, &report, sizeof(report));
	}
}
