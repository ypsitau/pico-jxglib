//==============================================================================
// ShellCmd_Net.cpp
//==============================================================================
#include "jxglib/Net.h"
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

}
