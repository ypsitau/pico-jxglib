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
	FAT::Flash fat1("a", 0x1010'0000, 0x0004'0000); // XIP address and size
	FAT::Flash fat2("b", 0x1014'0000, 0x0004'0000); // XIP address and size
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
