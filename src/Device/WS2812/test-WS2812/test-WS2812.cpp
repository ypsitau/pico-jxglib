#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Device/WS2812.h"

using namespace jxglib;

Device::WS2812 ws2812;

// callback function for ShellCmd_Device_WS2812.cpp
Device::WS2812& ShellCmd_Device_WS2812_GetWS2812() { return ws2812; }

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(false);
	while (true) {
		::jxglib_tick();
	}
}
