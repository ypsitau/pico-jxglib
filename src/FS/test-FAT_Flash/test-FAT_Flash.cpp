#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	FAT::Flash fat1("A", 0x1018'0000, 0x0004'0000);	// XIP address and size
	FAT::Flash fat2("B", 0x101c'0000, 0x0002'0000);	// XIP address and size
	FAT::Flash fat3("C", 0x0002'0000);				// XIP size
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
