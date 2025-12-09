#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "jxglib/USBDevice/Video.h"

#define FRAME_WIDTH   128
#define FRAME_HEIGHT  96
#define FRAME_RATE    10

using namespace jxglib;

void video_send_frame(void);

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
	USBDevice::Video video(deviceController, "UVC Control", "UVC Streaming", 0x81, FRAME_WIDTH, FRAME_HEIGHT, FRAME_RATE);
	deviceController.Initialize();
	for (;;) {
		video_send_frame();
		Tickable::Tick();
	}
}
