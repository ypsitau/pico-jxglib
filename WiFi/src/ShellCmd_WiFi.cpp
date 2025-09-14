//==============================================================================
// ShellCmd_WiFi.cpp
//==============================================================================
#include "jxglib/WiFi.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_WiFi {

ShellCmd(wifi, "controls WiFi")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [CMD]...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		return Result::Success;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		const char* value;
		if (Arg::GetAssigned(subcmd, "init", &value)) {
			if (::strcasecmp(value, "station") == 0 || ::strcasecmp(value, "sta") == 0){
				if (!WiFi::InitAsStation()) {
					printf("failed to initialise WiFi module\n");
					return Result::Error;
				}
			} else if (::strcasecmp(value, "access_point") == 0 || ::strcasecmp(value, "ap") == 0) {
				const char* ssid = nullptr;
				const char* password = nullptr;
				uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
				for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
					const char* subcmd = pSubcmdChild->GetProc();
					if (Arg::GetAssigned(subcmd, "ssid", &ssid)) {
						// nothing to do
					} else if (Arg::GetAssigned(subcmd, "password", &password)) {
						// nothing to do
					} else {
						terr.Printf("Unknown option: %s\n", subcmd);
						return Result::Error;
					}
					
				}
				if (!ssid) {
					terr.Printf("SSID is required for access_point mode\n");
					return Result::Error;
				}
				if (!password) {
					terr.Printf("Password is required for access_point mode\n");
					return Result::Error;
				}
				if (!WiFi::InitAsAccessPoint(ssid, password, auth)) {
					printf("failed to initialise WiFi module\n");
					return Result::Error;
				}
			} else {
				terr.Printf("Invalid mode: %s\n", value);
				return Result::Error;
			}
		} else if (::strcasecmp(subcmd, "deinit") == 0) {
			WiFi::Deinit();
		} else if (::strcasecmp(subcmd, "scan") == 0) {
			WiFi::Scan(tout);
		} else if (::strcasecmp(subcmd, "connect") == 0) {
			const char* ssid = nullptr;
			const char* password = nullptr;
			uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "ssid", &ssid)) {
					// nothing to do
				} else if (Arg::GetAssigned(subcmd, "password", &password)) {
					// nothing to do
				} else {
					terr.Printf("Unknown option: %s\n", subcmd);
					return Result::Error;
				}
				
			}
			if (!ssid) {
				terr.Printf("SSID is required for connect\n");
				return Result::Error;
			}
			WiFi::Connect(tout, ssid, password, auth);
		} else {
			terr.Printf("Unknown command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
