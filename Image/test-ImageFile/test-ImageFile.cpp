#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Display/WS2812.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	//Display::WS2812 display(16, 16); // default size
	//display.Initialize(GPIO::Instance(9));
	::jxglib_labo_init(false);
	while (true) ::jxglib_tick();
}
