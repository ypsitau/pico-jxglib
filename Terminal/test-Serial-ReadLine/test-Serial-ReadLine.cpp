#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

int main()
{
	::stdio_init_all();
	terminal.Initialize();
	//terminal.AttachInput(KeyboardStdio::Instance);
	//terminal.AttachOutput(PrintableStdio::Instance);
	terminal.Println("ReadLine Test Program");
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
	}
}
