#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/CmdLine.h"

using namespace jxglib;

Serial::Terminal terminal;

int main()
{
	::stdio_init_all();
	terminal.Initialize().AttachPrintable(Stdio::Instance).AttachKeyboard(Stdio::GetKeyboard());
	CmdLine::AttachTerminal(terminal);
	terminal.Println("CmdLine Test Program");
	for (;;) Tickable::Tick();
}
