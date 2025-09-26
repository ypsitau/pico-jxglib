//==============================================================================
// ShellCmd_Net.cpp
//==============================================================================
#include "jxglib/WiFi.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_Net {

ShellCmd(net, "controls Network")
{
	Net::WiFi& wifi = Net::WiFi::Instance;
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
		tout.Printf("  wifi-scan            scan for WiFi networks\n");
		tout.Printf("  wifi-ap  {ssid:SSID password:PASSWORD [auth:AUTH]}\n");
		tout.Printf("                       initialise as a WiFi access point\n");
		tout.Printf("  wifi-connect {ssid:SSID password:PASSWORD [timeout:SEC] [auth:AUTH]}\n");
		tout.Printf("                       connect to a WiFi access point\n");
		tout.Printf("  wifi-disconnect      disconnect from the current WiFi access point\n");
		tout.Printf("  config {[addr:ADDR] [netmask:NETMASK] [gateway:GATEWAY]}\n");
		tout.Printf("                       modify IP configuration\n");
		return Result::Success;
	}
	if (argc < 2) {
		wifi.PrintStatus(tout);
		return Result::Success;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	bool printConnectInfoFlag = false;
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		const char* value;
		if (::strcasecmp(subcmd, "wifi-scan") == 0) {
			std::unique_ptr<Net::WiFi::ScanResult> pScanResult(wifi.Scan());
			pScanResult->Print(tout);
		} else if (Arg::GetAssigned(subcmd, "wifi-ap", &value)) {
			const char* ssid = nullptr;
			const char* password = nullptr;
			uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "ssid", &value)) {
					ssid = value;
				} else if (Arg::GetAssigned(subcmd, "password", &value)) {
					password = value;
				} else if (Arg::GetAssigned(subcmd, "auth", &value)) {
					if (!Net::WiFi::StringToAuth(value, &auth)) {
						terr.Printf("Invalid auth: %s\n", value);
						return Result::Error;
					}
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
		} else if (::strcasecmp(subcmd, "wifi-connect") == 0) {
			char ssid[33] = {0};
			char password[65] = {0};
			const uint8_t* bssid = nullptr;
			uint32_t auth = CYW43_AUTH_WPA2_AES_PSK;
			//uint32_t auth = CYW43_AUTH_WPA2_MIXED_PSK;
			uint32_t msecTimeout = 15000; // 15 seconds
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
				} else if (Arg::GetAssigned(subcmd, "timeout", &value)) {
					int num = ::strtol(value, nullptr, 10);
					if (num <= 0) {
						terr.Printf("Invalid timeout: %s\n", value);
						return Result::Error;
					}
					msecTimeout = static_cast<uint32_t>(num) * 1000;
				} else if (Arg::GetAssigned(subcmd, "auth", &value)) {
					if (!Net::WiFi::StringToAuth(value, &auth)) {
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
			int errorCode = wifi.Connect(ssid, bssid, password, auth, msecTimeout);
			if (errorCode == PICO_ERROR_NONE) {
				printConnectInfoFlag = true;
			} else if (errorCode == PICO_ERROR_BADAUTH) {
				terr.Printf("Authentication failure for '%s'\n", ssid);
				return Result::Error;
			} else {
				terr.Printf("Failed to connect to '%s'\n", ssid);
				return Result::Error;
			}
		} else if (::strcasecmp(subcmd, "wifi-disconnect") == 0) {
			wifi.Disconnect();
			printConnectInfoFlag = true;
		} else if (::strcasecmp(subcmd, "config") == 0) {
			ip4_addr_t addr = wifi.GetAddr();
			ip4_addr_t netmask = wifi.GetNetmask();
			ip4_addr_t gateway = wifi.GetGateway();
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "addr", &value)) {
					if (!::ip4addr_aton(value, &addr)) {
						terr.Printf("Invalid IP address: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "netmask", &value)) {
					if (!::ip4addr_aton(value, &netmask)) {
						terr.Printf("Invalid netmask: %s\n", value);
						return Result::Error;
					}
				} else if (Arg::GetAssigned(subcmd, "gateway", &value)) {
					if (!::ip4addr_aton(value, &gateway)) {
						terr.Printf("Invalid gateway: %s\n", value);
						return Result::Error;
					}
				} else {
					terr.Printf("Unknown option: %s\n", subcmd);
					return Result::Error;
				}
			}
			if (ip4_addr_isany_val(addr) ||	ip4_addr_isany_val(netmask) || ip4_addr_isany_val(gateway)) {
				terr.Printf("addr, netmask and gateway are required for configuration\n");
				return Result::Error;
			}
			wifi.Configure(addr, netmask, gateway);
			printConnectInfoFlag = true;
		} else {
			terr.Printf("Unknown command: %s\n", subcmd);
			return Result::Error;
		}
	}
	if (printConnectInfoFlag) wifi.PrintStatus(terr);
	return Result::Success;
}

}
