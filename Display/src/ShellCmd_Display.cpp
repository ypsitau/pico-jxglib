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
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
