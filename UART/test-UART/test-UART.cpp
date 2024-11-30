#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Printable::SetStandardOutput(UART::Default);
	Dump.Cols(16)(reinterpret_cast<const void*>(0), 1000);
	//while (true) {
	//	Printf("Hello, world!\n");
	//	sleep_ms(1000);
	//}
}
