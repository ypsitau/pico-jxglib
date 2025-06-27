#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PWM.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
