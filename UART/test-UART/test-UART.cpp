#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	uint8_t buff[256];
	for (int i = 0; i < count_of(buff); i++) buff[i] = static_cast<uint8_t>(i + 0x10);
	::stdio_init_all();
	//const uint8_t* p = reinterpret_cast<const uint8_t*>(buff);
	////uint32_t num = (((static_cast<uint32_t>(p[0]) << 24) + (static_cast<uint32_t>(p[1]) << 16) +
	////	((static_cast<uint32_t>(p[2]) << 8) + static_cast<uint32_t>(p[3])));
	//uint32_t num = (static_cast<uint32_t>(p[0]) << 24) + (static_cast<uint32_t>(p[1]) << 16);
	//::printf("%08x\n", num);
#if 1
	Printable::SetStandardOutput(UART::Default);
	Println("--------");
	Dump.Data8Bit().Cols(16)(buff, 32);
	Println();
	Dump.Data16Bit().Cols(8)(buff, 16);
	Println();
	Dump.Data32Bit().Cols(4)(buff, 8);
	Println();
	Dump.Data64Bit().Cols(2)(buff, 4);
	Println();
	Dump.Data16BitBE().Cols(8)(buff, 16);
	Println();
	Dump.Data32BitBE().Cols(4)(buff, 8);
	Println();
	Dump.Data64BitBE().Cols(2)(buff, 4);
#endif
	//while (true) {
	//	Printf("Hello, world!\n");
	//	sleep_ms(1000);
	//}
}
