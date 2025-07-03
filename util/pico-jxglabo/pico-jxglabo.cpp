#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/I2C.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
