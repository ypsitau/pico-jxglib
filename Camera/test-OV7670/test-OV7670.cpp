#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PWM.h"
#include "jxglib/Display.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Camera/OV7670.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(true);
	Display::Base& display = Display::GetInstance(0);
	Camera::Base& camera = Camera::GetInstance(0);
	while (true) {
		const Image& image = camera.Capture();
		display.DrawImage(
			(display.GetWidth() - image.GetWidth()) / 2,
			(display.GetHeight() - image.GetHeight()) / 2, image);
		::jxglib_tick();
	}
}
