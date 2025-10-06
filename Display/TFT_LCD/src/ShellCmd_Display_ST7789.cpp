#include "jxglib/Shell.h"
#include "jxglib/Display/ST7789.h"

namespace jxglib::ShellCmd_ST7789 {

std::unique_ptr<ST7789> pDisplay;

ShellCmd_Named(display_st7789, "display-st7789", "Display::ST7789 display commands")
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
		terr.Printf(" setup {spi:SPI width:WIDTH height:HEIGHT rst:RST dc:DC cs:CS bl:BL}\n");
		terr.Printf("           Set up an Display::ST7789 display\n");
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
			if (pDisplay) {
				terr.Printf("Display::ST7789 display already set up.\n");
				return Result::Error;
			}
			spi_inst_t* spi = nullptr;
			int width = 0;
			int height = 0;
			uint pinRST = GPIO::InvalidPin;
			uint pinDC = GPIO::InvalidPin;
			uint pinCS = GPIO::InvalidPin;
			uint pinBL = GPIO::InvalidPin;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "spi", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= 2) {
						terr.Printf("invalid SPI number: %s\n", value);
						return Result::Error;
					}
					spi = (num == 0)? spi0 : spi1;
				} else if (Arg::GetAssigned(subcmd, "width", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num <= 0) {
						terr.Printf("invalid width: %s\n", value);
						return Result::Error;
					}
					width = num;
				} else if (Arg::GetAssigned(subcmd, "height", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num <= 0) {
						terr.Printf("invalid height: %s\n", value);
						return Result::Error;
					}
					height = num;
				} else if (Arg::GetAssigned(subcmd, "rst", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinRST = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "dc", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinDC = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "cs", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinCS = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "bl", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinBL = static_cast<uint>(num);
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (!spi || pinRST == GPIO::InvalidPin || pinDC == GPIO::InvalidPin ||
					pinCS == GPIO::InvalidPin || pinBL == GPIO::InvalidPin ||
					width <= 0 || height <= 0) {
				terr.Printf("spi, width, height, rst, dc, cs, and bl must be specified\n");
				return Result::Error;
			}
			pDisplay.reset(new ST7789(spi, width, height,
				{GPIO::Instance(pinRST), GPIO::Instance(pinDC), GPIO::Instance(pinCS), GPIO::Instance(pinBL)}));
		}
	}
	return Result::Success;
}

#if 0
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
#endif

}
