#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FAT_RAMDisk.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	FAT_RAMDisk fat(100 * 1024); // it seems 100kB is minimum
	fat.Mount();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
