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

ShellCmd_Named(sd_init, "sd-init", "Initialize SD card")
{
	if (sdCard.Initialize(true)) {
		terr.Printf("SD card initialized successfully.\n");
	} else {
		terr.Printf("Failed to initialize SD card.\n");
	}
	return 0;
}

ShellCmd_Named(sd_dump, "sd-dump", "prints SD card data at the specified sector")
{
	if (!sdCard.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return 1;
	}
	uint32_t lba = 0;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return 1;
		}
		lba = num;
	}
	uint8_t buff[512];
	sdCard.ReadBlock(lba, buff, 1);
	tout.Printf("Sector %d (0x%x)\n", lba, lba);
	Printable::DumpT(tout).DigitsAddr(4)(buff, sizeof(buff));
	return 0;
}

ShellCmd_Named(sd_mbr, "sd-mbr", "Read SD card MBR")
{
	if (!sdCard.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return 1;
	}
	uint8_t buff[512];
	sdCard.ReadBlock(0, buff, 1);
	SDCard::PrintMBR(tout, buff);
	return 0;
}

ShellCmd_Named(sd_write, "sd-write", "Write SD card sector")
{
	if (!sdCard.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return 1;
	}
	int lba = 0;
	if (argc > 1) {
		lba = ::atoi(argv[1]);
	}
	uint8_t buff[512];
	for (int i = 0; i < sizeof(buff); i++) buff[i] = static_cast<uint8_t>(i);
	sdCard.WriteBlock(lba, buff, 1);
	return 0;
}
