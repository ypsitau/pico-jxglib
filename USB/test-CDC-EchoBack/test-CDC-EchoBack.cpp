#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBD.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// EchoBack
//-----------------------------------------------------------------------------
class EchoBack : public USBD::CDC {
public:
	EchoBack(USBD::Device& device, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
				USBD::CDC(device, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10) {}
public:
	virtual void OnTask() override;
};

void EchoBack::OnTask()
{
	if (!cdc_available()) return;
	char buff[64];
	int bytes = cdc_read(buff, sizeof(buff));
	cdc_write(buff, bytes);
	cdc_write_flush();
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBD::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBD::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "CDC EchoBack", "CDC EchoBack Product", "0123456");
	EchoBack echoBack(device, "EchoBack Normal", 0x81, 0x02, 0x82);
	device.Initialize();
	for (;;) {
		device.Task();
	}
	return 0;
}
