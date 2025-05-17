#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FAT_Flash.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	FAT_Flash fat(0x00100000, 0x00080000);
	fat.Mount();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
