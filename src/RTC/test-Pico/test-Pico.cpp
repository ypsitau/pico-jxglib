#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/RTC/Pico.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

int main()
{
	stdio_init_all();
	RTC::Pico rtc;
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
