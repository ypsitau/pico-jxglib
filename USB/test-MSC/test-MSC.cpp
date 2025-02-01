#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USB.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USB::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USB::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
		iManufacturer:		0x01,
		iProduct:			0x02,
		iSerialNumber:		0x03,
	});
	USB::MSC msc(device, 0x03, 0x83);
	device.Initialize();
	for (;;) {
		device.Task();
	}
	return 0;
}
