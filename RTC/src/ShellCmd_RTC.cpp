#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/RTC.h"
#include "jxglib/Shell.h"
#include "jxglib/RTC/DS323x.h"
namespace jxglib::ShellCmd_RTC {

std::unique_ptr<RTC::Base> pInstance;

ShellCmd_Named(rtc_device, "rtc-device", "set up RTC device")
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
		if (::strcmp(subcmd, "dummy") == 0) {
			pInstance.reset(new RTC::Dummy);
		} else if (::strcmp(subcmd, "ds323x") == 0) {
			pInstance.reset(new RTC::DS323x(i2c0));
		} else {
			terr.Printf("Unknown subcommand: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

ShellCmd(rtc, "set or get RTC time")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("verbose",		'v',	"verbose output"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [DATE] [TIME]\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("\n"
				"Set or get the RTC time. If no argument is given, it prints the current time.\n");
		return Result::Error;
	}
	if (argc >= 2) {
		DateTime dt;
		RTC::Get(&dt);
		for (int iArg = 1; iArg < argc; iArg++) {
			if (DateTime::HasTimeFormat(argv[iArg])) {
				dt.ParseTime(argv[iArg]); // DateTime::HasTimeFormat() already ensures the validity
			} else if (!dt.Parse(argv[iArg])) {
				terr.Printf("Invalid date/time format. Use YYYY-MM-DD HH:MM:SS\n");
				return Result::Error;
			}
		}
		if (!RTC::Set(dt)) {
			terr.Printf("Failed to set RTC time to %s\n", RTC::GetDeviceName());
			return Result::Error;
		}
	}
	do {
		// get the current RTC time
		DateTime dt;
		if (!RTC::Get(&dt)) {
			terr.Printf("Failed to get RTC time from %s\n", RTC::GetDeviceName());
			return Result::Error;
		}
		terr.Printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
					dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.msec);
		if (arg.GetBool("verbose")) {
			terr.Printf("RTC Device: %s\n", RTC::GetDeviceName());
		}
	} while (0);
	return Result::Success;
}

}
