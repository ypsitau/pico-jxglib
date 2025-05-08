#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	//-------------------------------------------------------------------------
	Serial::Terminal terminal;
	terminal.Initialize();
	Shell::AttachTerminal(terminal);
	//-------------------------------------------------------------------------
	terminal.Println("Shell with Serial Terminal");
	for (;;) Tickable::Tick();
}
