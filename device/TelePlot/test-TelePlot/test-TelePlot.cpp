#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TelePlot.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	TelePlot telePlot(UART::Default);
}
