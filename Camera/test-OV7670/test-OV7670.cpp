#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(true);
	while (true) {
		::jxglib_tick();
	}
}
