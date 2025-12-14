#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "jxglib/USBDevice/VideoTransmitter.h"
#include "jxglib/Camera/OV7670.h"

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
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	::i2c_init(i2c0, 100000);
	Camera::OV7670 ov7670(i2c0, { D0: GPIO2, XCLK: GPIO10, PCLK: GPIO11, HREF: GPIO12, VSYNC: GPIO13 }, 24000000);
	ov7670.Initialize();
	ov7670
		.SetupReg()
		.EnableColorMode(true)
		.SetMirror(true)
		.SetResolution(Camera::Resolution::QQVGA)
		.SetFormat(Camera::Format::YUV422);
	int frameRate = 10;
	USBDevice::VideoTransmitter videoTransmitter(deviceController,
		"VideoTransmitter", "VideoTransmitter Streaming", 0x81, ov7670.GetSize(), frameRate);
	deviceController.Initialize();
	videoTransmitter.Initialize();
	for (;;) {
		if (videoTransmitter.CanTransmit()) {
			const Image& image = ov7670.Capture();
			videoTransmitter.Transmit(image.GetPointer());
		}
		Tickable::Tick();
	}
}
