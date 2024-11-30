#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	for (;;) {
		::printf("check\n");
	}
	//Printable::SetStandardOutput(UART::Default);
	//Dump.Data8Bit().Cols(16)(reinterpret_cast<const void*>(0), 32);
	//while (true) {
	//	Printf("Hello, world!\n");
	//	sleep_ms(1000);
	//}
}
