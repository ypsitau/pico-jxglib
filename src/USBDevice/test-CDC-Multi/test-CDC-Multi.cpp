#include <ctype.h>
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
	}, 0x0409, "CDC Test", "CDC Test Product", "0123456");
	USBDevice::CDC cdc1(deviceController, "EchoBack Normal", 0x81, 8, 0x02, 0x82, 64);
	USBDevice::CDC cdc2(deviceController, "EchoBack Upper", 0x83, 8, 0x04, 0x84, 64);
	USBDevice::CDC cdc3(deviceController, "EchoBack Crypt", 0x85, 8, 0x06, 0x86, 64);
	deviceController.Initialize();
	cdc1.Initialize();
	cdc2.Initialize();
	cdc3.Initialize();
	for (;;) {
		char buff[64];
		if (cdc1.cdc_available()) {
			int bytes = cdc1.cdc_read(buff, sizeof(buff));
			cdc1.cdc_write(buff, bytes);
			cdc1.cdc_write_flush();
		}
		if (cdc2.cdc_available()) {
			int bytes = cdc2.cdc_read(buff, sizeof(buff));
			for (int i = 0; i < bytes; i++) buff[i] = toupper(buff[i]);
			cdc2.cdc_write(buff, bytes);
			cdc2.cdc_write_flush();
		}
		if (cdc3.cdc_available()) {
			int bytes = cdc3.cdc_read(buff, sizeof(buff));
			for (int i = 0; i < bytes; i++) buff[i] = buff[i] + 1;
			cdc3.cdc_write(buff, bytes);
			cdc3.cdc_write_flush();
		}
		Tickable::Tick();
	}
}
