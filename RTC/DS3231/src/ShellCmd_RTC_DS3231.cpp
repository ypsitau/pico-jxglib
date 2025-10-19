#include <stdio.h>
#include "jxglib/RTC.h"
#include "jxglib/Shell.h"
#include "jxglib/RTC/DS3231.h"

namespace jxglib::ShellCmd_RTC_DS3231 {

std::unique_ptr<RTC::DS3231> pInstance;

ShellCmd_Named(rtc_ds3231, "rtc-ds3231", "set up RTC DS3231 device")
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
		terr.Printf("  setup  Set up a DS3231 device with the given parameters: {i2c:BUS}\n");
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
		if (Arg::GetAssigned(subcmd, "setup", &value)) {
			i2c_inst_t* i2c = nullptr;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "i2c", &value)) {
					int num = ::strtol(value, nullptr, 10);
					if (num < 0 || num > 1) {
						terr.Printf("Invalid I2C number: %s\n", value);
						return Result::Error;
					}
					i2c = ::i2c_get_instance(num);
				} else {
					terr.Printf("Unknown subcommand: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (!i2c) {
				terr.Printf("I2C number is not specified. Use 'i2c:N' subcommand.\n");
				return Result::Error;
			}
			pInstance.reset(new RTC::DS3231(i2c));
		} else {
			terr.Printf("Unknown subcommand: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
