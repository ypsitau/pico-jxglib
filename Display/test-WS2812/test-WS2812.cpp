#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Display/WS2812.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	jxglib_labo_init(false);
	Display::WS2812 display(16, 16);
	display.Initialize(GPIO15);
	display.Fill(Color::red);
	display.Refresh();
	while (true) {
		jxglib_tick();
	}
}
