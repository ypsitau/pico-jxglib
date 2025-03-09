#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/VT100.h"

using namespace jxglib;

int main()
{
	VT100::Terminal terminal(UART::Default);
	terminal.Initialize();
	::stdio_init_all();
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
	}
}
