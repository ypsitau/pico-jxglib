#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// EchoBack
//-----------------------------------------------------------------------------
class EchoBack : public USBDevice::CDC {
public:
	enum Mode { Normal, Upper, Crypt };
private:
	Mode mode_;
public:
	EchoBack(USBDevice& device, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, Mode mode) :
				USBDevice::CDC(device, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10), mode_{mode} {}
public:
	virtual void OnTick() override;
};

void EchoBack::OnTick()
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
	USBDevice device({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "CDC Test", "CDC Test Product", "0123456");
	EchoBack echoBack1(device, "EchoBack Normal", 0x81, 0x02, 0x82, EchoBack::Mode::Normal);
	EchoBack echoBack2(device, "EchoBack Upper", 0x83, 0x04, 0x84, EchoBack::Mode::Upper);
	EchoBack echoBack3(device, "EchoBack Crypt", 0x85, 0x06, 0x86, EchoBack::Mode::Crypt);
	device.Initialize();
	echoBack1.Initialize();
	echoBack2.Initialize();
	echoBack3.Initialize();
	for (;;) Tickable::Tick();
}
