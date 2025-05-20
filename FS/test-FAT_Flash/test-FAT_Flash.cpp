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
	FAT::Flash fat1(0x10100000, 0x00080000, "flash1"); // XIP address and size
	FAT::Flash fat2(0x10180000, 0x00080000, "flash2"); // XIP address and size
	//GPIO2.set_function_SPI0_SCK();
	//GPIO3.set_function_SPI0_TX();
	//GPIO4.set_function_SPI0_RX();
	//FAT::SDCard fat2(spi0, 10'000'000, {CS: GPIO5});
	fat1.Mount();
	fat2.Mount();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
