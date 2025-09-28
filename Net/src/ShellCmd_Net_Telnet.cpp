//==============================================================================
// ShellCmd_Net_Telnet.cpp
//==============================================================================
#include "jxglib/Shell.h"
#include "jxglib/Net/Telnet.h"

jxglib::Net::Telnet::Server& ShellCmd_Net_Telnet_GetTelnetServer();

namespace jxglib::ShellCmd_Net_Telnet {

ShellCmd_Named(telnet_server, "telnet-server", "controls telnet server")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		tout.Printf("Usage: %s [OPTION]... [CMD]...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf("  start			    start telnet server\n");
		tout.Printf("  stop			        stop telnet server\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Net::Telnet::Server& telnetServer = ShellCmd_Net_Telnet_GetTelnetServer();
	for (int iArg = 1; iArg < argc; ++iArg) {
		const char* subcmd = argv[iArg];
		if (::strcasecmp(subcmd, "start") == 0) {
			if (telnetServer.Start()) terr.Printf("Telnet server started\n");
		} else if (::strcasecmp(subcmd, "stop") == 0) {
			telnetServer.Stop();
			terr.Printf("Telnet server stopped\n");
		} else {
			terr.Printf("Unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}

