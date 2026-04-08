//==============================================================================
// ShellCmd_Net.cpp
//==============================================================================
#include "jxglib/NetUtil.h"
#include "jxglib/Shell.h"
#include "jxglib/RTC.h"

namespace jxglib::ShellCmd_Net {

ShellCmd(nslookup, "performs DNS lookup")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		terr.Printf("Usage: %s URL\n", GetName());
		arg.PrintHelp(terr);
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	ip_addr_t addr;
	if (!Net::DNS().GetHostByName(argv[1], &addr)) {
		terr.Printf("nslookup failed: %s\n", argv[1]);
		return Result::Error;
	}
	tout.Printf("%s\n", ::ip4addr_ntoa(&addr));
	return Result::Success;
}

Net::NTP ntp;

ShellCmd(ntp, "requests time from NTP server")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"print this help"),
		Arg::OptBool("utc",			'u',	"print UTC time"),
		Arg::OptBool("rtc",			'r',	"set RTC if available"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [SERVER]\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	DateTime dt;
	const char* urlServer = (argc < 2)? "pool.ntp.org" : argv[1];
	if (!ntp.GetTime(dt, urlServer)) {
		terr.Printf("NTP request failed: %s\n", ntp.GetErrorMsg());
		return Result::Error;
	}
	bool utcFlag = true;
	if (!arg.GetBool("utc")) {
		const char* strTZ = Shell::Instance.GetEnv("TZ");
		if (strTZ && *strTZ != '\0') {
			if (!dt.OffsetByTZ(strTZ)) {
				terr.Printf("timezone error: %s\n", strTZ);
				return Result::Error;
			}
			utcFlag = false;
		}
	}
	tout.Printf("%04d-%02d-%02d %02d:%02d:%02d%s\n",
				dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec, utcFlag? "Z" : "");
	if (arg.GetBool("rtc")) {
		RTC::Set(dt);
		terr.Printf("RTC updated\n");
	}
	return Result::Success;
}

ShellCmd(ping, "performs ping")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		terr.Printf("Usage: %s URL\n", GetName());
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	ip_addr_t addr;
	if (!Net::DNS().GetHostByName(argv[1], &addr)) {
		terr.Printf("nslookup failed: %s\n", argv[1]);
		return Result::Error;
	}
	uint32_t msecTimeout = 3000;
	Net::ICMP icmp;
	for (int i = 0; i < 3; i++) {
		uint32_t msecEcho;
		if (icmp.Echo(addr, &msecEcho, msecTimeout)) {
			tout.Printf("Reply from %s: time=%ums\n", ::ip4addr_ntoa(&addr), msecEcho);
		} else {
			terr.Printf("ping failed: %s\n", icmp.GetErrorMsg());
			return Result::Error;
		}
	}
	return Result::Success;
}

}
