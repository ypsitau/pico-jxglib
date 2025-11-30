#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PWM.h"
#include "jxglib/Display.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Device/OV7670.h"

using namespace jxglib;

Device::OV7670 ov7670(Device::OV7670::QVGA, Device::OV7670::Format::RGB565,
	i2c0, {DIN0: GPIO2, XLK: GPIO10, PLK: GPIO11, HREF: GPIO12, VSYNC: GPIO13}, 24000000);

jxglib::Device::OV7670& ShellCmd_Device_OV7670_GetOV7670()
{
	return ov7670;
}

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(false);
	//GPIO16.set_function_I2C0_SDA().pull_up();
	//GPIO17.set_function_I2C0_SCL().pull_up();
	//::i2c_init(i2c0, 100000);
	ov7670.Initialize();
	ov7670.SetupParam();
	Display::Base& display = Display::GetInstance(0);
	while (true) {
		Image& image = ov7670.Capture();
		display.DrawImage(
			(display.GetWidth() - image.GetWidth()) / 2,
			(display.GetHeight() - image.GetHeight()) / 2, image);
		::jxglib_tick();
	}
}
