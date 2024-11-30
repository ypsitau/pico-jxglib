#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	//uint8_t buff[256];
	//for (int i = 0; i < count_of(buff); i++) buff[i] = static_cast<uint8_t>(i + 0x10);
	const uint8_t* buff = reinterpret_cast<const uint8_t*>(0x10000000);
	Printable::SetStandardOutput(UART::Default);
	Println("--------");
	Dump.Data8Bit()(buff, 32);
	Println();
	Dump.Data16Bit()(buff, 16);
	Println();
	Dump.Data32Bit()(buff, 8);
	Println();
	Dump.Data64Bit()(buff, 4);
	Println();
	Dump.Data16BitBE()(buff, 16);
	Println();
	Dump.Data32BitBE()(buff, 8);
	Println();
	Dump.Data64BitBE()(buff, 4);
}
