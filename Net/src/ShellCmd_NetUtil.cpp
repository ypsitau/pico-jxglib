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
		Arg::OptBool("help",	'h',	"prints this help"),
		Arg::OptBool("rtc",		'r',	"set RTC if available"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s\n", GetName());
		return Result::Success;
	}
	DateTime dt;
	if (!ntp.GetTime(dt)) {
		terr.Printf("NTP request failed: %s\n", ntp.GetErrorMsg());
		return Result::Error;
	}
	tout.Printf("NTP time: %04d-%02d-%02d %02d:%02d:%02d\n",
				dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
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
