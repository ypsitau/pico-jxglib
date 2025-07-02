#include "jxglib/SDCard.h"
#include "jxglib/Shell.h"

using namespace jxglib;

SDCard& ShellCmd_SDCard_GetSDCard();

ShellCmd_Named(sd_init, "sd-init", "Initialize SD card")
{
	SDCard& sdCard = ShellCmd_SDCard_GetSDCard();
	if (sdCard.Initialize(true)) {
		terr.Printf("SD card initialized successfully.\n");
	} else {
		terr.Printf("Failed to initialize SD card.\n");
	}
	return Result::Success;
	
}

ShellCmd_Named(sd_dump, "sd-dump", "prints SD card data at the specified sector")
{
	SDCard& sdCard = ShellCmd_SDCard_GetSDCard();
	if (!sdCard.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return Result::Error;
	}
	uint32_t lba = 0;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return Result::Error;
		}
		lba = num;
	}
	uint8_t buff[512];
	sdCard.ReadBlock(lba, buff, 1);
	tout.Printf("Sector %d (0x%x)\n", lba, lba);
	Printable::DumpT(tout).DigitsAddr(4)(buff, sizeof(buff));
	return Result::Success;
}

ShellCmd_Named(sd_mbr, "sd-mbr", "Read SD card MBR")
{
	SDCard& sdCard = ShellCmd_SDCard_GetSDCard();
	if (!sdCard.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return Result::Error;
	}
	uint8_t buff[512];
	sdCard.ReadBlock(0, buff, 1);
	SDCard::PrintMBR(tout, buff);
	return Result::Success;
}

ShellCmd_Named(sd_write, "sd-write", "Write SD card sector")
{
	SDCard& sdCard = ShellCmd_SDCard_GetSDCard();
	if (!sdCard.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return Result::Error;
	}
	int lba = 0;
	if (argc > 1) {
		lba = ::atoi(argv[1]);
	}
	uint8_t buff[512];
	for (int i = 0; i < sizeof(buff); i++) buff[i] = static_cast<uint8_t>(i);
	sdCard.WriteBlock(lba, buff, 1);
	return Result::Success;
}
