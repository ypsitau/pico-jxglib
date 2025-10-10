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
			if (!value) {
				terr.Printf("missing color value\n");
				return Result::Error;
			}
			Color color;
			if (!color.Parse(value)) {
				terr.Printf("invalid color: %s\n", value);
				return Result::Error;
			}
			display.Fill(color);
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
	int iDisplay = 0;
	for (Display::Base* pDisplay = Display::Base::GetHead(); pDisplay; iDisplay++, pDisplay = pDisplay->GetNext()) {
		char remarks[128];
		pDisplay->GetRemarks(remarks, sizeof(remarks));
		terr.Printf("display%d: %s %s %dx%d%s%s\n", iDisplay, pDisplay->GetName(), pDisplay->GetVariantName(),
			pDisplay->GetWidth(), pDisplay->GetHeight(), remarks[0]? " " : "", remarks);
	}
}

}
