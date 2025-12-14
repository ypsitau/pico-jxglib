#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "jxglib/USBDevice/VideoTransmitter.h"

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
	uint8_t* buffFrame = reinterpret_cast<uint8_t*>(::malloc(width * height * 16 / 8));
	USBDevice::VideoTransmitter videoTransmitter(deviceController,
		"VideoTransmitter", "VideoTransmitter Streaming", 0x81, {width, height}, frameRate);
	deviceController.Initialize();
	videoTransmitter.Initialize();
	int startPos = 0;
	for (;;) {
		if (videoTransmitter.CanTransmit()) {
			static const uint8_t bar_color[8][4] = {
				//  Y,   U,   Y,   V
				{ 235, 128, 235, 128 }, // 100% White
				{ 219,  16, 219, 138 }, // Yellow
				{ 188, 154, 188,  16 }, // Cyan
				{ 173,  42, 173,  26 }, // Green
				{  78, 214,  78, 230 }, // Magenta
				{  63, 102,  63, 240 }, // Red
				{  32, 240,  32, 118 }, // Blue
				{  16, 128,  16, 128 }, // Black
			};
			uint8_t* end = &buffFrame[width * 2];
			unsigned idx = (width / 2 - 1) - (startPos % (width / 2));
			uint8_t* p = &buffFrame[idx * 4];
			for (unsigned i = 0; i < 8; ++i) {
				for (int j = 0; j < width / (2 * 8); ++j) {
					memcpy(p, &bar_color[i], 4);
					p += 4;
					if (end <= p) {
						p = buffFrame;
					}
				}
			}
			p = &buffFrame[width * 2];
			for (unsigned i = 1; i < height; ++i) {
				memcpy(p, buffFrame, width * 2);
				p += width * 2;
			}
			startPos++;
			videoTransmitter.Transmit(buffFrame);
		}
		Tickable::Tick();
	}
}
