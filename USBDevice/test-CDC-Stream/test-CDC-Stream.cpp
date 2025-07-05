#include "pico/stdlib.h"
#include "jxglib/USBDevice/CDC.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// CDC_EchoBack
//-----------------------------------------------------------------------------
class CDC_EchoBack : public USBDevice::CDC {
private:
	FIFOBuff<uint8_t, 64> buffRead_;
	FIFOBuff<uint8_t, 64> buffWrite_;
public:
	CDC_EchoBack(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
				USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10) {}
public:
	virtual void OnTick() override;
};

void CDC_EchoBack::OnTick()
{
	if (!cdc_available()) return;
	uint8_t buff[64];
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
	USBDevice::Controller deviceController({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "CDC EchoBack", "CDC EchoBack Product", "0123456");
	CDC_EchoBack cdc(deviceController, "EchoBack Normal", 0x81, 0x02, 0x82);
	deviceController.Initialize();
	cdc.Initialize();
	for (;;) Tickable::Tick();
}
