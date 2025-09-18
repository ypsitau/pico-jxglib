//==============================================================================
// ShellCmd_WiFi.cpp
//==============================================================================
#include "jxglib/WiFi.h"
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
		tout.Printf("  static {addr:ADDR netmask:NETMASK gateway:GATEWAY}\n");
		tout.Printf("                       set static IP configuration\n");
		tout.Printf("  connect {ssid:SSID password:PASSWORD [auth:AUTH]}\n");
		tout.Printf("                       connect to a WiFi network\n");
		return Result::Success;
	}
	if (argc < 2) {
		WiFi::PrintConnectInfo(tout, wifi.GetConnectInfo());
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
					printf("Failed to initialise WiFi module\n");
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
					printf("Failed to initialise WiFi module\n");
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
		} else if (::strcasecmp(subcmd, "dhcp") == 0) {
			wifi.SetDHCP();
		} else if (::strcasecmp(subcmd, "static") == 0) {
			const char* strAddr = nullptr;
			const char* strNetmask = nullptr;
			const char* strGateway = nullptr;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "addr", &value)) {
					strAddr = value;
				} else if (Arg::GetAssigned(subcmd, "netmask", &value)) {
					strNetmask = value;
				} else if (Arg::GetAssigned(subcmd, "gateway", &value)) {
					strGateway = value;
				} else {
					terr.Printf("Unknown option: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (strAddr && strNetmask && strGateway) {
				ip4_addr_t addr, netmask, gateway;
				if (!::ip4addr_aton(strAddr, &addr)) {
					terr.Printf("Invalid IP address: %s\n", strAddr);
					return Result::Error;
				}
				if (!::ip4addr_aton(strNetmask, &netmask)) {
					terr.Printf("Invalid netmask: %s\n", strNetmask);
					return Result::Error;
				}
				if (!::ip4addr_aton(strGateway, &gateway)) {
					terr.Printf("Invalid gateway: %s\n", strGateway);
					return Result::Error;
				}
				wifi.SetStatic(addr, netmask, gateway);
			} else if (strAddr || strNetmask || strGateway) {
				terr.Printf("addr, netmask and gateway must be specified together\n");
				return Result::Error;
			}
		} else if (::strcasecmp(subcmd, "connect") == 0) {
			char ssid[33] = {0};
			char password[65] = {0};
			const uint8_t* bssid = nullptr;
			uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "ssid", &value)) {
					::snprintf(ssid, sizeof(ssid), "%s", value);
					Tokenizer::RemoveSurroundingQuotes(ssid);
				} else if (Arg::GetAssigned(subcmd, "bssid", &value)) {
					//bssid = value;
				} else if (Arg::GetAssigned(subcmd, "password", &value)) {
					::snprintf(password, sizeof(password), "%s", value);
					Tokenizer::RemoveSurroundingQuotes(password);
				} else if (Arg::GetAssigned(subcmd, "auth", &value)) {
					if (!WiFi::StringToAuth(value, &auth)) {
						terr.Printf("Invalid auth: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("Unknown option: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (ssid[0] == '\0') {
				terr.Printf("SSID is required for connect\n");
				return Result::Error;
			}
			if (password[0] == '\0') {
				terr.Printf("Password is required for access_point mode\n");
				return Result::Error;
			}
			int linkStat = wifi.Connect(ssid, bssid, password, auth);
			if (linkStat == CYW43_LINK_UP) {
				WiFi::PrintConnectInfo(terr, wifi.GetConnectInfo());
			} else if (linkStat == CYW43_LINK_BADAUTH) {
				terr.Printf("Authentication failure for '%s'\n", ssid);
			} else {
				terr.Printf("Failed to connect to '%s'\n", ssid);
			}
		} else if (::strcasecmp(subcmd, "disconnect") == 0) {
			wifi.Disconnect();
			terr.Printf("Disconnected\n");
		} else {
			terr.Printf("Unknown command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
