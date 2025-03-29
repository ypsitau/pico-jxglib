#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/CmdLine.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

CmdLineEntry(ls)
{
	terminal.Printf("hoge\n");
}

int main()
{
	::stdio_init_all();
	terminal.Initialize();
	CmdLine::AttachTerminal(terminal);
	for (;;) Tickable::Tick();
}
