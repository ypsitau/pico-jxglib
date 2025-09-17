//==============================================================================
// ShellCmd_Net.cpp
//==============================================================================
#include "jxglib/NetUtil.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_Net {

ShellCmd(nslookup, "performs DNS lookup")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		tout.Printf("Usage: %s URL\n", GetName());
		return Result::Success;
	}
	ip_addr_t addr;
	if (Net::DNS().GetHostByName(argv[1], &addr)) {
		tout.Printf("%s\n", ::ip4addr_ntoa(&addr));
	} else {
		tout.Printf("nslookup failed: %s\n", argv[1]);
	}
	return Result::Success;
}

ShellCmd(ntp, "requests time from NTP server")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s\n", GetName());
		return Result::Success;
	}
	DateTime dt;
	if (Net::NTP().GetTime(dt)) {
		tout.Printf("NTP time: %04d-%02d-%02d %02d:%02d:%02d\n",
				dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
	} else {
		tout.Printf("NTP request failed\n");
	}
	return Result::Success;
}

}
