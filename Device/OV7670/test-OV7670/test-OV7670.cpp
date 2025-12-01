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
	::jxglib_labo_init(true);
	ov7670.Initialize();
	//ov7670.SetupRegisters();

	ov7670.ResetAllRegisters();

	//ov7670.WriteReg(0x12, (1 << 1));

	Display::Base& display = Display::GetInstance(0);
	while (true) {
		Image& image = ov7670.Capture();
		uint8_t* p = image.GetPointer();
		for (int i = 0; i < image.GetBytesBuff(); i += 4, p += 4) {
			uint8_t y1 = p[0];
			uint8_t u = p[1];
			uint8_t y2 = p[2];
			uint8_t v = p[3];
			do {
				int16_t r = static_cast<int16_t>(y1 + 1.402 * (v - 128));
				int16_t g = static_cast<int16_t>(y1 - 0.344136 * (u - 128) - 0.714136 * (v - 128));
				int16_t b = static_cast<int16_t>(y1 + 1.772 * (u - 128));
				r = (r < 0)? 0 : (r > 255)? 255 : r;
				g = (g < 0)? 0 : (g > 255)? 255 : g;
				b = (b < 0)? 0 : (b > 255)? 255 : b;
				*reinterpret_cast<uint16_t*>(p) = ColorRGB565(r, g, b);
			} while (0);
			do {
				int16_t r = static_cast<int16_t>(y2 + 1.402 * (v - 128));
				int16_t g = static_cast<int16_t>(y2 - 0.344136 * (u - 128) - 0.714136 * (v - 128));
				int16_t b = static_cast<int16_t>(y2 + 1.772 * (u - 128));
				r = (r < 0)? 0 : (r > 255)? 255 : r;
				g = (g < 0)? 0 : (g > 255)? 255 : g;
				b = (b < 0)? 0 : (b > 255)? 255 : b;
				*reinterpret_cast<uint16_t*>(p + 2) = ColorRGB565(r, g, b);
			} while (0);
		}

		display.DrawImage(
			(display.GetWidth() - image.GetWidth()) / 2,
			(display.GetHeight() - image.GetHeight()) / 2, image);
		::jxglib_tick();
	}
}
