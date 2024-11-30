#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	const void* p = reinterpret_cast<const void*>(0x10000000);
	::stdio_init_all();
	Printable::SetStandardOutput(UART::Default);
	Println("--------");
	Dump.Data8Bit().Cols(16)(p, 32);
	Println();
	Dump.Data16Bit().Cols(8)(p, 16);
	Println();
	Dump.Data32Bit().Cols(4)(p, 8);
	Println();
	Dump.Data64Bit().Cols(2)(p, 4);
	Println();
	Dump.Data16BitBE().Cols(8)(p, 16);
	Println();
	Dump.Data32BitBE().Cols(4)(p, 8);
	Println();
	Dump.Data64BitBE().Cols(2)(p, 4);
	//while (true) {
	//	Printf("Hello, world!\n");
	//	sleep_ms(1000);
	//}
}
