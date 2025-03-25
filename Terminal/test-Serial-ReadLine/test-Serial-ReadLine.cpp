#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

TickableEntry(hoge, 300)
{
	static bool value = false;
	value = !value;
	GPIO2.put(value);
}

int main()
{
	::stdio_init_all();
	GPIO2.init().set_dir_OUT();
	terminal.Initialize();
	terminal.AttachInput(Stdio::Keyboard::Instance);
	terminal.AttachOutput(Stdio::Printable::Instance);
	terminal.Println("ReadLine Test Program");
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
		//::printf("%d\n", terminal.GetChar());
	}
}
