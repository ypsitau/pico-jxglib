#include "jxglib/Shell.h"
#include "jxglib/USBDevice/Serial.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBDevice::Controller deviceController({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "CDC Stream", "CDC Stream Product", "0123456");
	USBDevice::Serial serial(deviceController, "CDC Stream", 0x81, 0x02, 0x82);
	deviceController.Initialize();
	serial.Initialize();
	Serial::Terminal terminal;
	terminal.AttachKeyboard(serial.GetKeyboard()).AttachPrintable(serial);
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
