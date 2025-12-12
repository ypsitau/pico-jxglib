#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "jxglib/USBDevice/Video.h"
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
	Camera::OV7670 ov7670(
		i2c0, { D0: GPIO2, XCLK: GPIO10, PCLK: GPIO11, HREF: GPIO12, VSYNC: GPIO13 }, 24000000);
	ov7670.Initialize();
	ov7670.SetupReg();
	ov7670.EnableColorMode(true);
	ov7670.SetResolution(Camera::OV7670::Resolution::QQVGA).SetFormat(Camera::OV7670::Format::YUV422);
	int width = 160, height = 120, frameRate = 10;
	USBDevice::VideoSimple video(deviceController, "UVC Control", "UVC Streaming", 0x81, width, height, frameRate);
	deviceController.Initialize();
	video.Initialize();
	for (;;) {
		if (video.CanTransferFrame()) {
			const Image& image = ov7670.Capture();
			video.TransferFrame(image.GetPointer());
		}
		Tickable::Tick();
	}
}
