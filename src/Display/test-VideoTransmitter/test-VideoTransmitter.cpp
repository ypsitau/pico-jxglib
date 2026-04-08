#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Display/VideoTransmitter.h"

using namespace jxglib;

Display::VideoTransmitter display;

ShellCmd(refresh, "Refresh VideoTransmitter display")
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
	while (true) {
		jxglib_tick();
	}
}
