#include "jxglib/Shell.h"
#include "jxglib/Display/ST7735.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Display/ILI9488.h"
#include "jxglib/DrawableTest.h"

namespace jxglib::ShellCmd_TFT_LCD {

ShellCmd_Named(display_tft_lcd, "display-tft-lcd", "TFT LCD display commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" setup {spi:PIN rst:PIN dc:PIN cs:PIN [bl:PIN] [size:WxH]}\n");
		terr.Printf("           Set up parameters for a display\n");
		terr.Printf(" init      Initialize the display\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	Display::TFT_LCD* pDisplay = nullptr;;
	const char* value;
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
			if (pDisplay) {
				terr.Printf("display already set up.\n");
				return Result::Error;
			}
			spi_inst_t* spi = nullptr;
			Size size;
			uint pinRST = GPIO::InvalidPin;
			uint pinDC = GPIO::InvalidPin;
			uint pinCS = GPIO::InvalidPin;
			uint pinBL = GPIO::InvalidPin;
			Display::Dir dir = Display::Dir::Normal;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "spi", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= 2) {
						terr.Printf("invalid SPI number: %s\n", value);
						return Result::Error;
					}
					spi = (num == 0)? spi0 : spi1;
				} else if (Arg::GetAssigned(subcmd, "size", &value)) {
					if (!value) {
						terr.Printf("size not specified\n");
						return Result::Error;
					}
					if (!size.Parse(value) || size.width <= 0 || size.height <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "rst", &value)) {
					if (!value) {
						terr.Printf("rst not specified\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinRST = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "dc", &value)) {
					if (!value) {
						terr.Printf("dc not specified\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinDC = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "cs", &value)) {
					if (!value) {
						terr.Printf("cs not specified\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinCS = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "bl", &value)) {
					if (!value) {
						terr.Printf("bl not specified\n");
						return Result::Error;
					}
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinBL = static_cast<uint>(num);
				} else if (Arg::GetAssigned(subcmd, "dir", &value)) {
					if (!value) {
						terr.Printf("dir not specified\n");
						return Result::Error;
					}
					if (::strcasecmp(value, "normal") == 0) {
						dir = Display::Dir::Normal;
					} else if (::strcasecmp(value, "rotate90") == 0) {
						dir = Display::Dir::Rotate90;
					} else if (::strcasecmp(value, "rotate180") == 0) {
						dir = Display::Dir::Rotate180;
					} else if (::strcasecmp(value, "rotate270") == 0) {
						dir = Display::Dir::Rotate270;
					} else if (::strcasecmp(value, "mirror-horz") == 0) {
						dir = Display::Dir::MirrorHorz;
					} else if (::strcasecmp(value, "mirror-vert") == 0) {
						dir = Display::Dir::MirrorVert;
					} else {
						terr.Printf("invalid dir: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (!spi || pinRST == GPIO::InvalidPin || pinDC == GPIO::InvalidPin || pinCS == GPIO::InvalidPin) {
				terr.Printf("spi, rst, dc, cs, and bl must be specified\n");
				return Result::Error;
			}
			Display::TFT_LCD::PinAssign pinAssign = {GPIO::Instance(pinRST), GPIO::Instance(pinDC), GPIO::Instance(pinCS), GPIO::Instance(pinBL)};
			if (::strcmp(argv[0], "display-st7735") == 0) {
				pDisplay = new Display::ST7735(spi, size.width, size.height, pinAssign);
			} else if (::strcmp(argv[0], "display-st7735-b") == 0) {
				pDisplay = new Display::ST7735::TypeB(spi, size.width, size.height, pinAssign);
			} else if (::strcmp(argv[0], "display-st7789") == 0) {
				pDisplay = new Display::ST7789(spi, size.width, size.height, pinAssign);
			} else if (::strcmp(argv[0], "display-ili9341") == 0) {
				pDisplay = new Display::ILI9341(spi, size.width, size.height, pinAssign);
			} else if (::strcmp(argv[0], "display-ili9488") == 0) {
				pDisplay = new Display::ILI9488(spi, size.width, size.height, pinAssign);
			} else {
				terr.Printf("unknown command name: %s\n", argv[0]);
				return Result::Error;
			}
			pDisplay->Initialize(dir);
		} else if (::strcasecmp(subcmd, "test") == 0) {
			if (!pDisplay) {
				terr.Printf("display not set up.\n");
				return Result::Error;
			}
			DrawableTest::DrawString(*pDisplay);
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

ShellCmdAlias_Named(display_st7735, "display-st7735", display_tft_lcd)
ShellCmdAlias_Named(display_st7735_b, "display-st7735-b", display_tft_lcd)
ShellCmdAlias_Named(display_st7789, "display-st7789", display_tft_lcd)
ShellCmdAlias_Named(display_ili9341, "display-ili9341", display_tft_lcd)
ShellCmdAlias_Named(display_ili9488, "display-ili9488", display_tft_lcd)

}
