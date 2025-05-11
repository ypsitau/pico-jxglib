#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/LFS.h"
#include "jxglib/Flash.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal);
	for (;;) Tickable::Tick();
}
