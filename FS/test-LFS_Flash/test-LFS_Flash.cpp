#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/LFS_Flash.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

LFS_Flash lfs(0x00100000, 0x00010000); // XIP address and size

int main()
{
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
