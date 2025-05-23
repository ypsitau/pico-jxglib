#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/SDCard.h"

using namespace jxglib;

FAT::SDCard fat("sd", spi0, 10 * 1000 * 1000, {CS: GPIO5});

SDCard& GetSDCard() { return fat.GetSDCard(); }	// callback for ShellCmd_SDCard

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
