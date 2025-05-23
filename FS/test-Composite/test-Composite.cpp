#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/FAT/SDCard.h"
#include "jxglib/FAT/RAMDisk.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	LFS::Flash lfs1("a", 0x1018'0000, 0x0004'0000); // XIP address and size
	LFS::Flash lfs2("b", 0x101c'0000, 0x0004'0000); // XIP address and size
	FAT::Flash fat1("c", 0x1010'0000, 0x0004'0000); // XIP address and size
	FAT::Flash fat2("d", 0x1014'0000, 0x0004'0000); // XIP address and size
	FAT::SDCard fat3("e", spi0, 10'000'000, {CS: GPIO5});
	//FAT::SDCard fat4("f", spi0, 10'000'000, {CS: GPIO6});
	FAT::RAMDisk fat5("g", 100'000);
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
