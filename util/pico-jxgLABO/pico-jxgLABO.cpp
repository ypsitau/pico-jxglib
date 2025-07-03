#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/I2C.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	FAT::Flash drive("*C:", 0x1010'0000, 0x0010'0000); // Flash address and size 1MB
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
