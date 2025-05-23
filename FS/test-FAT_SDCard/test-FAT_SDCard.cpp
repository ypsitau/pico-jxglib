#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/SDCard.h"

using namespace jxglib;

FAT::SDCard fat("sd", spi0, 10'000'000, {CS: GPIO5});

SDCard& ShellCmd_SDCard_GetSDCard() { return fat.GetSDCard(); }	// callback for ShellCmd_SDCard

int main()
{
	bi_decl(bi_1pin_with_name(GPIO2, "SD Card SCK"));
	bi_decl(bi_1pin_with_name(GPIO3, "SD Card MOSI"));
	bi_decl(bi_1pin_with_name(GPIO4, "SD Card MISO"));
	bi_decl(bi_1pin_with_name(GPIO5, "SD Card CS"));
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
