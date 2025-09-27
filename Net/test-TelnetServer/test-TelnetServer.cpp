#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main()
{
	stdio_init_all();
	//jxglib_enable_startup_script(false);
	jxglib_labo_init(false);
	while (true) {
		jxglib_tick();
	}
}
