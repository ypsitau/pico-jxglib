#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FAT_SDCard.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;


int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	FAT_SDCard fat(spi0, 10 * 1000 * 1000, {CS: GPIO5});
	fat.Mount();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
