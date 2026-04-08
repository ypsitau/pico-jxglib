#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/RAMDisk.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	FAT::RAMDisk fat("RAM", 100 * 1024); // it seems 100kB is minimum
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
