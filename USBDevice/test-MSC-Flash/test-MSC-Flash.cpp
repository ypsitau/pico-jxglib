#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBDevice/Flash.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBDevice::Controller deviceController({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,		// Use Interface Class (Composite Device)
		bDeviceSubClass:	0x00,		// Unused
		bDeviceProtocol:	0x00,		// Unused
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "RPi Flash", "RPi Flash Device", "3141592653");
	USBDevice::Flash flash(deviceController, 0x1010'0000, 0x010'0000, 0x01, 0x81);
	
	deviceController.Initialize();
	//flash.Initialize();
	FAT::Flash fat("*C:", 0x1010'0000, 0x010'0000);
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
