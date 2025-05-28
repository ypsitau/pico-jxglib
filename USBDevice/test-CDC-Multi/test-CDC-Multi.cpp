#include <ctype.h>
#include "pico/stdlib.h"
#include "jxglib/USBDevice/CDC.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// CDC_EchoBack
//-----------------------------------------------------------------------------
class CDC_EchoBack : public USBDevice::CDC {
public:
	enum Mode { Normal, Upper, Crypt };
private:
	Mode mode_;
public:
	CDC_EchoBack(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, Mode mode) :
				USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10), mode_{mode} {}
public:
	virtual void OnTick() override;
};

void CDC_EchoBack::OnTick()
{
	if (!cdc_available()) return;
	char buff[64];
	int bytes = cdc_read(buff, sizeof(buff));
	switch (mode_) {
	case Mode::Normal: {
		break;
	}
	case Mode::Upper: {
		for (int i = 0; i < bytes; i++) buff[i] = toupper(buff[i]);
		break;
	}
	case Mode::Crypt: {
		for (int i = 0; i < bytes; i++) buff[i] = buff[i] + 1;
		break;
	}
	default: break;
	}
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
	}, 0x0409, "CDC Test", "CDC Test Product", "0123456");
	CDC_EchoBack cdc1(deviceController, "EchoBack Normal", 0x81, 0x02, 0x82, CDC_EchoBack::Mode::Normal);
	CDC_EchoBack cdc2(deviceController, "EchoBack Upper", 0x83, 0x04, 0x84, CDC_EchoBack::Mode::Upper);
	CDC_EchoBack cdc3(deviceController, "EchoBack Crypt", 0x85, 0x06, 0x86, CDC_EchoBack::Mode::Crypt);
	deviceController.Initialize();
	cdc1.Initialize();
	cdc2.Initialize();
	cdc3.Initialize();
	for (;;) Tickable::Tick();
}
