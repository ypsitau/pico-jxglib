#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Display/WS2812.h"

using namespace jxglib;

Display::WS2812 display(16, 16);

Device::WS2812& ShellCmd_Device_WS2812_GetWS2812() { return display.GetDevice(); }

ShellCmd(refresh, "Refresh WS2812 display")
{
	//display.Fill(Color::red);
	display.SetFont(FontSet::GetInstance(0));
	display.DrawString(0, 0, "AB");
	display.Refresh();
	return Result::Success;
}

int main()
{
	::stdio_init_all();
	jxglib_labo_init(false);
	display.Initialize(GPIO15);
	while (true) {
		jxglib_tick();
	}
}
