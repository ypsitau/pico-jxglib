#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "jxglib/USBDevice/Video.h"

using namespace jxglib;

//--------------------------------------------------------------------+
// Main
//--------------------------------------------------------------------+
int main(void)
{
	USBDevice::Controller deviceController({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "Video Test", "Video Test Product", "0123456");
	int width = 80, height = 60, frameRate = 10;
	uint8_t* frameBuffer = reinterpret_cast<uint8_t*>(::malloc(width * height * 16 / 8));
	USBDevice::VideoAttachable video(deviceController, "UVC Control", "UVC Streaming", 0x81, width, height, frameRate, frameBuffer);
	deviceController.Initialize();
	video.Initialize();
	for (;;) Tickable::Tick();
}
