//==============================================================================
// ShellCmd_WiFi.cpp
//==============================================================================
#include "jxglib/WiFi.h"
#include "jxglib/Common.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

jxglib::WiFi& ShellCmd_WiFi_GetWiFi();

namespace jxglib::ShellCmd_WiFi {

ShellCmd(wifi, "controls WiFi")
{
	WiFi& wifi = ShellCmd_WiFi_GetWiFi();
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
		tout.Printf("  init[:type]          initialize WiFi module (type:station|access_point, default:station)\n");
		tout.Printf("  deinit               deinitialize WiFi module\n");
		tout.Printf("  scan                 scan for WiFi networks\n");
		tout.Printf("  connect {ssid:SSID password:PASSWORD}\n");
		tout.Printf("                       connect to a WiFi network\n");
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
			if (!value || ::strcasecmp(value, "station") == 0 || ::strcasecmp(value, "sta") == 0){
				if (!wifi.InitAsStation()) {
					printf("failed to initialise WiFi module\n");
					return Result::Error;
				}
			} else if (::strcasecmp(value, "access_point") == 0 || ::strcasecmp(value, "ap") == 0) {
				const char* ssid = nullptr;
				const char* password = nullptr;
				uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
				for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
					const char* subcmd = pSubcmdChild->GetProc();
					if (Arg::GetAssigned(subcmd, "ssid", &value)) {
						ssid = value;
					} else if (Arg::GetAssigned(subcmd, "password", &value)) {
						password = value;
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
				if (!wifi.InitAsAccessPoint(ssid, password, auth)) {
					printf("failed to initialise WiFi module\n");
					return Result::Error;
				}
			} else {
				terr.Printf("Invalid mode: %s\n", value);
				return Result::Error;
			}
		} else if (::strcasecmp(subcmd, "deinit") == 0) {
			wifi.Deinit();
		} else if (::strcasecmp(subcmd, "scan") == 0) {
			wifi.Scan(tout);
		} else if (::strcasecmp(subcmd, "connect") == 0) {
			const char* ssid = nullptr;
			const char* password = nullptr;
			uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
			//CYW43_AUTH_WPA_TKIP_PSK
			//CYW43_AUTH_WPA2_AES_PSK
			//CYW43_AUTH_WPA2_MIXED_PSK
			//CYW43_AUTH_WPA3_SAE_AES_PSK
			//CYW43_AUTH_WPA3_WPA2_AES_PSK
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "ssid", &value)) {
					ssid = value;
				} else if (Arg::GetAssigned(subcmd, "password", &value)) {
					password = value;
				} else {
					terr.Printf("Unknown option: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (!ssid) {
				terr.Printf("SSID is required for connect\n");
				return Result::Error;
			}
			if (!password) {
				terr.Printf("Password is required for access_point mode\n");
				return Result::Error;
			}
			tout.Printf("'%s' '%s'\n", ssid, password);
			wifi.Connect(tout, ssid, password, auth);
		} else {
			terr.Printf("Unknown command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
