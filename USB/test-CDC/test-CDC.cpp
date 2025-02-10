#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBD.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// EchoBack
//-----------------------------------------------------------------------------
class EchoBack : public USBD::CDC {
public:
	EchoBack(USBD::Device& device, const char* str, uint8_t endpNotif, uint8_t bytesNotif, 
			uint8_t endpBulkOut, uint8_t endpBulkIn, uint8_t bytesBulk, uint8_t pollingInterval) :
		USBD::CDC(device, str, endpBulkIn, bytesBulk, endpBulkIn, endpBulkIn, bytesBulk, pollingInterval) {}
public:
	virtual void On_line_state(bool dtr, bool rts) = 0;
	virtual void On_rx() = 0;
};

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBD::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBD::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "CDC Test", "CDC Test Product", "0123456");
	device.Initialize();
	for (;;) {
		device.Task();
	}
	return 0;
}
