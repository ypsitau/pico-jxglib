#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

int main()
{
	stdio_init_all();
	::jxglib_labo_init(false);
	while (true) {
		::jxglib_tick();
	}
}
