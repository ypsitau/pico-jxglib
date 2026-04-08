#include "pico/stdlib.h"
#include "jxglib/USBDevice/CDC.h"

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
	}, 0x0409, "CDC EchoBack", "CDC EchoBack Product", "01234567");
	USBDevice::CDC cdc(deviceController, "EchoBack Normal", 0x81, 8, 0x02, 0x82, 64);
	deviceController.Initialize();
	cdc.Initialize();
	for (;;) {
		if (cdc.cdc_available()) {
			char buff[64];
			int bytes = cdc.cdc_read(buff, sizeof(buff));
			cdc.cdc_write(buff, bytes);
			cdc.cdc_write_flush();
		}
		Tickable::Tick();
	}
}
