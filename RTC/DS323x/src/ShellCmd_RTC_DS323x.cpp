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
		terr.Printf("Subcommands:\n");
		terr.Printf("  i2c:N        I2C number (0 or 1)\n");
		return Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	const char* value;
	uint idxI2C = static_cast<uint>(-1);
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (Arg::GetAssigned(subcmd, "i2c", &value)) {
			int num = ::strtol(value, nullptr, 10);
			if (num < 0 || num > 1) {
				terr.Printf("Invalid I2C number: %s\n", value);
				return Result::Error;
			}
			idxI2C = static_cast<uint>(num);
		} else {
			terr.Printf("Unknown subcommand: %s\n", subcmd);
			return Result::Error;
		}
	}
	if (idxI2C == static_cast<uint>(-1)) {
		terr.Printf("I2C number is not specified. Use 'i2c:N' subcommand.\n");
		return Result::Error;
	}
	pInstance.reset(new RTC::DS323x(::i2c_get_instance(idxI2C)));
	return Result::Success;
}

}
