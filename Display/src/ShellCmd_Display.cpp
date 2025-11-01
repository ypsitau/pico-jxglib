#include "jxglib/SPI.h"
#include "jxglib/Shell.h"
#include "jxglib/Display.h"

namespace jxglib::ShellCmd_Display {

void PrintDisplays(Printable& terr);

int iDisplay = 0;

struct {
	Point pos {0, 0};
	Size size {0, 0};
	Color color;
} rectFill;

struct {
	Point pos {0, 0};
	Size size {0, 0};
	Color color;
} rect;

struct {
	Point pos {0, 0};
	char str[128] = "";
} text;

struct {
	Point pos {0, 0};
	Size size {0, 0};
	Point offset {0, 0};
	Image image;
} blit;

ShellCmd_Named(ls_display, "ls-display", "lists all displays")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	PrintDisplays(terr);
	return Result::Success;
}

ShellCmd(dr, "draw commands on displays")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("display",	'd',	"specifies display number (default: 0)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	const char* value = nullptr;
	if (arg.GetString("display", &value)) {
		char* endptr;
		int num = ::strtol(value, &endptr, 10);
		if (*endptr != '\0' || num < 0) {
			terr.Printf("invalid display number: %s\n", argv[1]);
			return Result::Error;
		}
		iDisplay = num;
	}
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	Display::Base& display = Display::GetInstance(iDisplay);
	if (!display.IsValid()) {
		terr.Printf("display #%d is not available\n", iDisplay);
		return Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
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
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!rectFill.pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "size", &value)) {
					if (!value) {
						terr.Printf("missing size value\n");
						return Result::Error;
					}
					if (!rectFill.size.Parse(value) || rectFill.size.width <= 0 || rectFill.size.height <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "color", &value)) {
					if (!value) {
						terr.Printf("missing color value\n");
						return Result::Error;
					}
					if (!rectFill.color.Parse(value)) {
						terr.Printf("invalid color: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawRectFill(Rect(rectFill.pos, rectFill.size), rectFill.color);
			display.Refresh();
		} else if (Arg::GetAssigned(subcmd, "rect", &value)) {
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!rect.pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "size", &value)) {
					if (!value) {
						terr.Printf("missing size value\n");
						return Result::Error;
					}
					if (!rect.size.Parse(value) || rect.size.width <= 0 || rect.size.height <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "color", &value)) {
					if (!value) {
						terr.Printf("missing color value\n");
						return Result::Error;
					}
					if (!rect.color.Parse(value)) {
						terr.Printf("invalid color: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawRect(Rect(rect.pos, rect.size), rect.color);
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
		} else if (Arg::GetAssigned(subcmd, "text", &value)) {
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!text.pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "str", &value)) {
					if (!value) {
						terr.Printf("missing str value\n");
						return Result::Error;
					}
					::snprintf(text.str, sizeof(text.str), "%s", value);
					Tokenizer::RemoveSurroundingQuotes(text.str);
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawStringWrap(text.pos, text.str);
			display.Refresh();
#if 0
		} else if (Arg::GetAssigned(subcmd, "blit", &value)) {
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!blit.pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			display.DrawStringWrap(text.pos, text.str);
			display.Refresh();
#endif
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
