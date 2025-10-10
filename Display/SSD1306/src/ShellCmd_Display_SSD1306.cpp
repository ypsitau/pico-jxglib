#include "jxglib/I2C.h"
#include "jxglib/Shell.h"
#include "jxglib/Display/SSD1306.h"

namespace jxglib::ShellCmd_Display_SSD1306 {

ShellCmd_Named(display_ssd1306, "display-ssd1306", "SSD1306 display commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool genericFlag = (::strcasecmp(argv[0], "display-tftlcd") == 0);
	if (arg.GetBool("help") || argc < 2) {
		terr.Printf("Usage: %s [OPTION]...%s CMDS...\n", GetName(), genericFlag? " DISPLAY" : "");
		arg.PrintHelp(terr);
		if (genericFlag) {
			terr.Printf("Available Displays: st7735, st7735-b st7789, ili9341, ili9488\n");
		}
		terr.Printf("Sub Commands:\n");
		terr.Printf(" setup     Set up parameters for a display.\n");
		terr.Printf("           Parameters: {spi:PIN rst:PIN dc:PIN cs:PIN [bl:PIN] [size:WxH]}\n");
		terr.Printf(" init      Initialize the display\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	Display::SSD1306* pDisplay = nullptr;;
	const char* value;
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
			if (pDisplay) {
				terr.Printf("display already set up.\n");
				return Result::Error;
			}
			int iI2C = -1;
			Size size;
			Display::Dir dir = Display::Dir::Normal;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "i2c", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= 2) {
						terr.Printf("invalid I2C number: %s\n", value);
						return Result::Error;
					}
					iI2C = num;
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
			if (iI2C < 0) {
				terr.Printf("i2c must be specified\n");
				return Result::Error;
			}
			I2C& i2c = I2C::get_instance(iI2C);
			pDisplay = new Display::SSD1306(i2c, size.width, size.height);
			pDisplay->Initialize();
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
