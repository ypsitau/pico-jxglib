#include "jxglib/Device/SDCard.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/SDCard.h"

namespace jxglib::ShellCmd_Device_SDCard {

std::unique_ptr<FAT::SDCard> pFAT;

ShellCmd(sdcard, "SD card commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" setup  Setup an SD card with the given parameters:\n");
		terr.Printf("          {spi:SPI cs:CS [drive:DRIVE] [baudrate:BAUDRATE]}\n");
		terr.Printf(" init   Initialize the SD card\n");
		return Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	const char* value;
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
			if (pFAT) {
				terr.Printf("SD card already set up.\n");
				return Result::Error;
			}
			char driveName[32] = "SDCard";
			spi_inst_t* spi = nullptr;
			uint pinCS = GPIO::InvalidPin;
			uint baudrate = 10'000'000;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "spi", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= 2) {
						terr.Printf("invalid SPI number: %s\n", value);
						return Result::Error;
					}
					spi = (num == 0)? spi0 : spi1;
				} else if (Arg::GetAssigned(subcmd, "cs", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinCS = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "drive", &value)) {
					::snprintf(driveName, sizeof(driveName), "%s", value);
					Tokenizer::RemoveSurroundingQuotes(driveName);
				} else if (Arg::GetAssigned(subcmd, "baudrate", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0) {
						terr.Printf("invalid baudrate: %s\n", value);
						return Result::Error;
					}
					baudrate = static_cast<uint>(num);
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (!spi || pinCS == GPIO::InvalidPin) {
				terr.Printf("spi and cs must be specified\n");
				return Result::Error;
			}
			pFAT.reset(new FAT::SDCard(driveName, spi, baudrate, {GPIO::Instance(pinCS)}));
		} else if (::strcasecmp(subcmd, "init") == 0) {
			if (!pFAT) {
				terr.Printf("SD card not set up. Execute 'setup' subcommand first.\n");
				return Result::Error;
			}
			Device::SDCard& sdCardDev = pFAT->GetSDCardDev();
			if (sdCardDev.Initialize(true)) {
				terr.Printf("SD card initialized successfully.\n");
			} else {
				terr.Printf("Failed to initialize SD card.\n");
			}
		}
	}
	return Result::Success;
}

#if 0
ShellCmd_Named(sd_dump, "sd-dump", "prints SD card data at the specified sector")
{
	Device::SDCard& sdCardDev = ShellCmd_Device_SDCard_GetSDCard();
	if (!sdCardDev.IsInitialized()) {
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
	sdCardDev.ReadBlock(lba, buff, 1);
	tout.Printf("Sector %d (0x%x)\n", lba, lba);
	Printable::DumpT(tout).DigitsAddr(4)(buff, sizeof(buff));
	return Result::Success;
}

ShellCmd_Named(sd_mbr, "sd-mbr", "Read SD card MBR")
{
	Device::SDCard& sdCardDev = ShellCmd_Device_SDCard_GetSDCard();
	if (!sdCardDev.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return Result::Error;
	}
	uint8_t buff[512];
	sdCardDev.ReadBlock(0, buff, 1);
	SDCard::PrintMBR(tout, buff);
	return Result::Success;
}

ShellCmd_Named(sd_write, "sd-write", "Write SD card sector")
{
	Device::SDCard& sdCardDev = ShellCmd_Device_SDCard_GetSDCard();
	if (!sdCardDev.IsInitialized()) {
		terr.Printf("SD card not initialized. Execute sd-init first.\n");
		return Result::Error;
	}
	int lba = 0;
	if (argc > 1) {
		lba = ::atoi(argv[1]);
	}
	uint8_t buff[512];
	for (int i = 0; i < sizeof(buff); i++) buff[i] = static_cast<uint8_t>(i);
	sdCardDev.WriteBlock(lba, buff, 1);
	return Result::Success;
}
#endif

}
