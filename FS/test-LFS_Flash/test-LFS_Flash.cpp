#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	LFS::Flash lfs(0x10100000, 0x00010000); // XIP address and size
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
