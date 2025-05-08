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
	Shell::AttachTerminal(terminal.Initialize());
	//-------------------------------------------------------------------------
	terminal.Println("Shell Test Program with Serial Terminal");
	for (;;) Tickable::Tick();
}
