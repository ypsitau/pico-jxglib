#include "jxglib/SPI.h"
#include "jxglib/Shell.h"
#include "jxglib/Display.h"

namespace jxglib::ShellCmd_Display {

void PrintDisplays(Printable& terr);

ShellCmd(display, "display commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DISPLAY CMDS...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	if (argc < 2) {
		PrintDisplays(terr);
		return Result::Success;
	}
	int iDisplay = 0;
	do {
		char* endptr;
		iDisplay = ::strtol(argv[1], &endptr, 10);
		if (*endptr != '\0' || iDisplay < 0) {
			terr.Printf("invalid display number: %s\n", argv[1]);
			return Result::Error;
		}
	} while (0);
	Display::Base& display = Display::GetInstance(iDisplay);
	if (!display.IsValid()) {
		terr.Printf("invalid display number: %d\n", iDisplay);
		return Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[2], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	const char* value;
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (Arg::GetAssigned(subcmd, "fill", &value)) {
			Color color;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "color", &value)) {
					if (!value) {
						terr.Printf("missing color value\n");
						return Result::Error;
					}
					if (!color.Parse(value)) {
						terr.Printf("invalid color: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.Fill(color);
			display.Refresh();
		} else if (Arg::GetAssigned(subcmd, "rect-fill", &value)) {
			Point pos;
			Size size;
			Color color;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "size", &value)) {
					if (!value) {
						terr.Printf("missing size value\n");
						return Result::Error;
					}
					if (!size.Parse(value) || size.width <= 0 || size.height <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "color", &value)) {
					if (!value) {
						terr.Printf("missing color value\n");
						return Result::Error;
					}
					if (!color.Parse(value)) {
						terr.Printf("invalid color: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawRectFill(Rect(pos, size), color);
			display.Refresh();
		} else if (Arg::GetAssigned(subcmd, "rect", &value)) {
			Point pos;
			Size size;
			Color color;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "size", &value)) {
					if (!value) {
						terr.Printf("missing size value\n");
						return Result::Error;
					}
					if (!size.Parse(value) || size.width <= 0 || size.height <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "color", &value)) {
					if (!value) {
						terr.Printf("missing color value\n");
						return Result::Error;
					}
					if (!color.Parse(value)) {
						terr.Printf("invalid color: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawRect(Rect(pos, size), color);
			display.Refresh();
		} else if (Arg::GetAssigned(subcmd, "font", &value)) {
			if (!value) {
				terr.Printf("missing font name\n");
				return Result::Error;
			}
			const FontSet& fontSet = FontSet::GetInstance(value);
			if (fontSet.IsNone()) {
				terr.Printf("font not found: %s\n", value);
				return Result::Error;
			}
			display.SetFont(fontSet);
		} else if (Arg::GetAssigned(subcmd, "string", &value)) {
			Point pos;
			char str[128] = "";
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "str", &value)) {
					if (!value) {
						terr.Printf("missing str value\n");
						return Result::Error;
					}
					::snprintf(str, sizeof(str), "%s", value);
					Tokenizer::RemoveSurroundingQuotes(str);
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawStringWrap(pos, str);
			display.Refresh();
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

void PrintDisplays(Printable& terr)
{
	Display::Base* pDisplay = Display::Base::GetHead();
	if (!pDisplay) {
		terr.Printf("no displays\n");
		return;
	}
	for (int iDisplay = 0; pDisplay; iDisplay++, pDisplay = pDisplay->GetNext()) {
		const char* variantName = pDisplay->GetVariantName();
		bool variantFlag = (variantName[0] != '\0');
		char remarks[128];
		pDisplay->GetRemarks(remarks, sizeof(remarks));
		bool remarksFlag = (remarks[0] != '\0');
		terr.Printf("display %d: %s%s%s%s %dx%d%s%s\n",
			iDisplay, pDisplay->GetName(),
			variantFlag? " [" : "", variantName, variantFlag? "]" : "",
			pDisplay->GetWidth(), pDisplay->GetHeight(),
			remarksFlag? " " : "", remarks);
	}
}

}
