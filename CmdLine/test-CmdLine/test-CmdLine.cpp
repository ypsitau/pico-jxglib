#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/CmdLine.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

CmdLineEntry(dump)
{
	terminal.Dump(reinterpret_cast<const void*>(0x10000000), 64);
}

int main()
{
	::stdio_init_all();
	terminal.Initialize();
	CmdLine::AttachTerminal(terminal);
	for (;;) Tickable::Tick();
}
