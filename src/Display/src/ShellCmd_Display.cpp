#include "jxglib/SPI.h"
#include "jxglib/Shell.h"
#include "jxglib/Display.h"
#include "jxglib/ImageFile.h"

namespace jxglib::ShellCmd_Display {

void ListDisplays(Printable& tout);

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
	int nRepeatX = 1;
	int nRepeatY = 1;
} image;

ShellCmd(display, "controls displays")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
		//Arg::OptString("index",	'i',	"specifies display index (default: 0)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... COMMAND...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Commands:\n");
		terr.Printf(" list           lists all displays\n");
		return Result::Success;
	}
	if (argc < 2) {
		ListDisplays(tout);
		return Result::Success;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "list") == 0) {
			ListDisplays(tout);
			return Result::Success;
		}
	}
	return Result::Error;
}

ShellCmd(draw, "draw commands on displays")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
		Arg::OptString("index",	'i',	"specifies display index (default: 0)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	const char* value = nullptr;
	if (arg.GetBool("help") || argc < 2) {
		terr.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		terr.Printf("Commands:\n");
		terr.Printf(" fill {color:COLOR}                                fills the display with the specified color\n");
		terr.Printf(" rect-fill {color:COLOR pos:X,Y size:WIDTHxHEIGHT} fills a rectangle with the specified color\n");
		terr.Printf(" rect {color:COLOR pos:X,Y size:WIDTHxHEIGHT}      draws a rectangle with the specified color\n");
		terr.Printf(" font:NAME                                         sets the font to be used for text drawing\n");
		terr.Printf(" text {pos:X,Y str:STRING}                         draws the specified text\n");
		terr.Printf(" image-load:FILE                                   loads an image from the specified file\n");
		terr.Printf(" image {pos:X,Y pos-shift:X,Y size:WIDTHxHEIGHT offset:X,Y offset-shift:X,Y repeat-x:N repeat-y:N}\n");
		terr.Printf("                                                   draws the loaded image\n");
		arg.PrintHelp(terr);
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	if (arg.GetString("index", &value)) {
		char* endptr;
		int num = ::strtol(value, &endptr, 10);
		if (*endptr != '\0' || num < 0) {
			terr.Printf("invalid display index: %s\n", argv[1]);
			return Result::Error;
		}
		iDisplay = num;
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
		} else if (Arg::GetAssigned(subcmd, "image-load", &value)) {
			if (!value) {
				terr.Printf("file name is not specified\n");
				return Result::Error;
			}
			char fileName[FS::MaxPath];
			::snprintf(fileName, sizeof(fileName), "%s", value);
			Tokenizer::RemoveSurroundingQuotes(fileName);
			std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
			if (!pFile) {
				terr.Printf("Failed to open file: %s\n", fileName);
				return Result::Error;
			}
			if (!ImageFile::Read(image.image, *pFile)) {
				terr.Printf("Failed to read image file: %s\n", fileName);
				return Result::Error;
			}
			image.size = Size(0, 0);
			image.offset = Point(0, 0);
		} else if (Arg::GetAssigned(subcmd, "image", &value)) {
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "pos", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!image.pos.Parse(value)) {
						terr.Printf("invalid pos: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "pos-shift", &value)) {
					if (!value) {
						terr.Printf("missing pos-shift value\n");
						return Result::Error;
					}
					Point posTrans;
					if (!posTrans.Parse(value)) {
						terr.Printf("invalid pos-shift: %s\n", value);
						return Result::Error;
					}
					image.pos += posTrans;
				} else if (Arg::GetAssigned(subcmd, "size", &value)) {
					if (!value) {
						terr.Printf("missing size value\n");
						return Result::Error;
					}
					if (!image.size.Parse(value) || image.size.width <= 0 || image.size.height <= 0) {
						terr.Printf("invalid size: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "offset", &value)) {
					if (!value) {
						terr.Printf("missing pos value\n");
						return Result::Error;
					}
					if (!image.offset.Parse(value)) {
						terr.Printf("invalid offset: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "offset-shift", &value)) {
					if (!value) {
						terr.Printf("missing offset-shift value\n");
						return Result::Error;
					}
					Point offsetTrans;
					if (!offsetTrans.Parse(value)) {
						terr.Printf("invalid offset-shift: %s\n", value);
						return Result::Error;
					}
					image.offset += offsetTrans;
					if (image.size.width <= 0) {
						// nothing to do
					} else if (image.offset.x < 0) {
						image.offset.x = image.image.GetWidth() - image.size.width;
						if (image.offset.x < 0) image.offset.x = 0;
					} else if (image.offset.x + image.size.width > image.image.GetWidth()) {
						image.offset.x = 0;
					}
					if (image.size.height <= 0) {
						// nothing to do
					} else if (image.offset.y < 0) {
						image.offset.y = image.image.GetHeight() - image.size.height;
						if (image.offset.y < 0) image.offset.y = 0;
					} else if (image.offset.y + image.size.height > image.image.GetHeight()) {
						image.offset.y = 0;
					}
				} else if (Arg::GetAssigned(subcmd, "repeat-x", &value)) {
					if (value) {
						int num = ::strtol(value, nullptr, 0);
						if (num <= 0) {
							terr.Printf("invalid repeat-x value: %s\n", value);
							return Result::Error;
						}
						image.nRepeatX = num;
					} else {
						image.nRepeatX = -1;
					}
				} else if (Arg::GetAssigned(subcmd, "repeat-y", &value)) {
					if (value) {
						int num = ::strtol(value, nullptr, 0);
						if (num <= 0) {
							terr.Printf("invalid repeat-y value: %s\n", value);
							return Result::Error;
						}
						image.nRepeatY = num;
					} else {
						image.nRepeatY = -1;
					}
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
			Size sizeImageAdj = image.size.IsZero()? Size(image.image.GetWidth() - image.offset.x, image.image.GetHeight() - image.offset.y) : image.size;
			if (sizeImageAdj.width > 0 && sizeImageAdj.height > 0) {
				int y = image.pos.y;
				for (int iRepeatY = 0; iRepeatY < image.nRepeatY || image.nRepeatY < 0; iRepeatY++, y += sizeImageAdj.height) {
					if (y >= display.GetHeight()) break;
					int x = image.pos.x;
					for (int iRepeatX = 0; iRepeatX < image.nRepeatX || image.nRepeatX < 0; iRepeatX++, x += sizeImageAdj.width) {
						if (x >= display.GetWidth()) break;
						display.DrawImage(x, y, image.image, Rect(image.offset, sizeImageAdj));
					}
				}
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "sleep", &value)) {
			if (!value) {
				terr.Printf("specify a sleep duration in milliseconds\n");
				return false;
			}
			int msec = ::strtol(value, nullptr, 0);
			if (msec <= 0) {
				terr.Printf("Invalid sleep duration: %s\n", value);
				return false;
			}
			Tickable::Sleep(msec);
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
		if (!pSubcmd->GetNext()) display.Refresh();
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

void ListDisplays(Printable& tout)
{
	Display::Base* pDisplay = Display::Base::GetHead();
	if (!pDisplay) {
		tout.Printf("no displays\n");
		return;
	}
	for (int iDisplay = 0; pDisplay; iDisplay++, pDisplay = pDisplay->GetNext()) {
		const char* variantName = pDisplay->GetVariantName();
		bool variantFlag = (variantName[0] != '\0');
		char remarks[128];
		pDisplay->GetRemarks(remarks, sizeof(remarks));
		bool remarksFlag = (remarks[0] != '\0');
		tout.Printf("display#%d: %s%s%s%s %dx%d%s%s\n",
			iDisplay, pDisplay->GetName(),
			variantFlag? " [" : "", variantName, variantFlag? "]" : "",
			pDisplay->GetWidth(), pDisplay->GetHeight(),
			remarksFlag? " " : "", remarks);
	}
}

}
