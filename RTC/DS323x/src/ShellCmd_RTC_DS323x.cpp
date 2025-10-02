#include <stdio.h>
#include "jxglib/RTC.h"
#include "jxglib/Shell.h"
#include "jxglib/RTC/DS323x.h"

namespace jxglib::ShellCmd_RTC_DS323x {

std::unique_ptr<RTC::DS323x> pInstance;

ShellCmd_Named(rtc_ds323x, "rtc-ds323x", "set up RTC DS323x device")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
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
		if (::strcmp(subcmd, "i2c") == 0) {
		} else {
			terr.Printf("Unknown subcommand: %s\n", subcmd);
			return Result::Error;
		}
	}
	pInstance.reset(new RTC::DS323x(i2c0));
	return Result::Success;
}

}
