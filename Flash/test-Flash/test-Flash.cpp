#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/Flash.h"

using namespace jxglib;

int main()
{
	uint8_t buff[256];
	for (int i = 0; i < sizeof(buff); i++) buff[i] = static_cast<uint8_t>(i);
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	terminal.Println("Flash Test Program");
	Flash::Write(0x00100000, buff, 256);
	Flash::Sync();
	for (;;) Tickable::Tick();
}
