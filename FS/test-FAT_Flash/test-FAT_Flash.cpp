#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/FAT/SDCard.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	FAT::Flash fat1("a", 0x1010'0000, 0x0008'0000); // XIP address and size
	FAT::Flash fat2("b", 0x1018'0000, 0x0008'0000); // XIP address and size
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	FAT::SDCard fat3("c", spi0, 10'000'000, {CS: GPIO5});
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
