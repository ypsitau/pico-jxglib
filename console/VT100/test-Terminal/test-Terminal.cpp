#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/VT100.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	VT100::Terminal terminal(PrintableStdio::Instance);
	terminal.Initialize();
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
	}
}
