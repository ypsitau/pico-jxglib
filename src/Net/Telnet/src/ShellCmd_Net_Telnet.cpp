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
		Arg::OptBool("quiet",		'q',	"do not print status and error messages"),
		Arg::OptString("port",		'p',	"port number (default: 23)", "PORT"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [CMD]...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" start          start telnet server\n");
		tout.Printf(" stop           stop telnet server\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Printable& terrOpt = arg.GetBool("quiet")? PrintableDumb::Instance : terr;
	Net::Telnet::Server& telnetServer = ShellCmd_Net_Telnet_GetTelnetServer();
	const char* value;
	if (arg.GetString("port", &value)) {
		int port = ::strtol(value, nullptr, 0);
		if (port < 1 || port > 65535) {
			terrOpt.Printf("invalid port number: %s\n", value);
			return Result::Error;
		}
		if (telnetServer.IsRunning()) {
			terrOpt.Printf("cannot change port number while telnet server is running\n");
			return Result::Error;
		}
		telnetServer.SetPort(static_cast<uint16_t>(port));
	}
	bool printStatusFlag = true;
	for (int iArg = 1; iArg < argc; ++iArg) {
		const char* subcmd = argv[iArg];
		if (::strcasecmp(subcmd, "start") == 0) {
			if (!telnetServer.Start()) {
				terrOpt.Printf("failed to start telnet server\n");
				return Result::Error;
			}
		} else if (::strcasecmp(subcmd, "stop") == 0) {
			telnetServer.Stop();
		} else {
			terrOpt.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	if (!printStatusFlag) {
		// nothing to do
	} else if (telnetServer.IsRunning()) {
		terrOpt.Printf("telnet server is running on port %u\n", telnetServer.GetPort());
	} else {
		terrOpt.Printf("telnet server is not running\n");
	}
	return Result::Success;
}

}

