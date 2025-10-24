#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(false);
	GPIO11.init().set_dir_OUT().put(true);
	//ws2812.Initialize(GPIO12);

	while (true) {
		::jxglib_tick();
	}
}
