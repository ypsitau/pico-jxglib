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
	EchoBack(USBD::Device& device) : USBD::CDC(device, "CDC", 0x81, 8, 0x02, 0x82, 64, 10) {}
public:
	void Initialize() {}
public:
	virtual void OnTask() override;
	virtual void On_line_state(bool dtr, bool rts) override {}
	virtual void On_rx() override {}
};

void EchoBack::OnTask()
{
	if (::tud_cdc_available()) {
		char buff[64];
		uint32_t bytes = ::tud_cdc_read(buff, sizeof(buff));
		::tud_cdc_write(buff, bytes);
		::tud_cdc_write_flush();
	}
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
	}, 0x0409, "CDC Test", "CDC Test Product", "0123456");
	EchoBack echoBack(device);
	device.Initialize();
	echoBack.Initialize();
	for (;;) {
		device.Task();
	}
	return 0;
}
