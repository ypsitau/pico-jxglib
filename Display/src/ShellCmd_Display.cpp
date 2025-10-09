#include "jxglib/SPI.h"
#include "jxglib/Shell.h"
#include "jxglib/Display.h"

namespace jxglib::ShellCmd_Display {

ShellCmd(display, "display commands")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}

	int iDisplay = 0;
	for (Display::Base* pDisplay = Display::Base::GetHead(); pDisplay; iDisplay++, pDisplay = pDisplay->GetNext()) {
		char remarks[128];
		pDisplay->GetRemarks(remarks, sizeof(remarks));
		terr.Printf("display%d: %s %s %dx%d%s%s\n", iDisplay, pDisplay->GetName(), pDisplay->GetVariantName(),
			pDisplay->GetWidth(), pDisplay->GetHeight(), remarks[0]? " " : "", remarks);
	}

	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
		} else if (::strcasecmp(subcmd, "test") == 0) {
			//if (!pDisplay) {
			//	terr.Printf("display not set up.\n");
			//	return Result::Error;
			//}
			//DrawableTest::DrawString(*pDisplay);
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
