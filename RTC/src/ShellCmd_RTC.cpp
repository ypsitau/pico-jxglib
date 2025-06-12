#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/RTC.h"
#include "jxglib/Shell.h"

using namespace jxglib;

ShellCmd(rtc, "set or get RTC time")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("verbose",		'v',	"verbose output"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [OPTION]... [DATE] [TIME]\nOptions:\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("\n"
				"Set or get the RTC time. If no argument is given, it gets the current time.\n");
		return 1;
	}
	if (argc >= 2) {
		DateTime dt;
		RTC::Get(&dt);
		for (int iArg = 1; iArg < argc; iArg++) {
			if (DateTime::IsTime(argv[iArg])) {
				// If the argument is a time, set it
				if (!dt.ParseTime(argv[iArg])) {
					terr.Printf("Invalid time format. Use HH:MM:SS\n");
					return 1;
				}
			} else {
				// Otherwise, parse the date
				if (!dt.ParseDate(argv[iArg])) {
					terr.Printf("Invalid date format. Use YYYY-MM-DD HH:MM:SS\n");
					return 1;
				}
			}
		}
		if (!RTC::Set(dt)) {
			terr.Printf("Failed to set RTC time to %s\n", RTC::GetDeviceName());
			return 1;
		}
	}
	do {
		// get the current RTC time
		DateTime dt;
		if (!RTC::Get(&dt)) {
			terr.Printf("Failed to get RTC time from %s\n", RTC::GetDeviceName());
			return 1;
		}
		terr.Printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
					dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, dt.msec);
		if (arg.GetBool("verbose")) {
			terr.Printf("RTC Device: %s\n", RTC::GetDeviceName());
		}
	} while (0);
	return 0;
}
