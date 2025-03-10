#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/VT100.h"

using namespace jxglib;

VT100::Terminal terminal;

int main()
{
	::stdio_init_all();
	terminal.Initialize();
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
	}
}
