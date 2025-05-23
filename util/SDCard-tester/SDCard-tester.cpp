#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "jxglib/Stream.h"
#include "jxglib/SDCard.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

SDCard sdCard(spi0, 10 * 1000 * 1000, {CS: GPIO5});	// 10MHz

SDCard& GetSDCard() { return sdCard; } // callback function for ShellCmd_SDCard

int main()
{
	//bi_decl(bi_3pins_with_func(GPIO2, GPIO3, GPIO4, GPIO_FUNC_SPI));
	bi_decl(bi_1pin_with_name(GPIO2, "SD Card SCK"));
	bi_decl(bi_1pin_with_name(GPIO3, "SD Card MOSI"));
	bi_decl(bi_1pin_with_name(GPIO4, "SD Card MISO"));
	bi_decl(bi_1pin_with_name(GPIO5, "SD Card CS"));
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	Serial::Terminal terminal; Shell::AttachTerminal(terminal.Initialize());
	Shell::SetPrompt("SDCard>");
	for (;;) Tickable::Tick();
}
