#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBDevice/MSCDrive.h"
#include "jxglib/USBDevice/CDCSerial.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

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
	}, 0x0409, "jxglib", "pico-jxgLABO", "000000000000");
	FAT::Flash fat("*G:", PICO_FLASH_SIZE_BYTES - 0x010'0000);
	if (!fat.Mount()) fat.Format();
	USBDevice::MSCDrive mscDrive(deviceController, 0x01, 0x81);
	USBDevice::CDCSerial cdcSerial(deviceController, "CDCSerial", 0x82, 0x03, 0x83);
	deviceController.Initialize();
	mscDrive.Initialize(fat);
	cdcSerial.Initialize();
	::adc_init();
	::adc_set_temp_sensor_enabled(true);
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
