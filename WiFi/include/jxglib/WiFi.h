//==============================================================================
// jxglib/WiFi.h
//==============================================================================
#ifndef PICO_JXGLIB_WIFI_H
#define PICO_JXGLIB_WIFI_H
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "jxglib/Common.h"

//------------------------------------------------------------------------------
// WiFi
//------------------------------------------------------------------------------
namespace jxglib {
	
class WiFi {
public:
	class Polling : public Tickable {
	private:
		WiFi& wifi_;
	public:
		Polling(WiFi& wifi) : wifi_(wifi) {}
	public:
		virtual const char* GetTickableName() const override { return "WiFi::Polling"; }
		virtual void OnTick() override { wifi_.Poll(); }
	};
private:
	uint32_t country_;
	bool initializedFlag_;
	bool connectedFlag_;
	Polling polling_;
public:
	WiFi(uint32_t country = PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE);
public:
	bool InitAsStation();
	bool InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth = CYW43_AUTH_WPA2_AES_PSK);
	void Deinit();
	void Poll();
	void Scan(Printable& tout);
	int Connect(Printable& tout, const char* ssid, const uint8_t* bssid, const char* password, uint32_t auth);
	void Disconnect();
public:
	static const ip_addr_t* GetIPAddr() { return netif_ip_addr4(netif_default); }
	static const ip_addr_t* GetNetmask() { return netif_ip_netmask4(netif_default); }
	static const ip_addr_t* GetGateway() { return netif_ip_gw4(netif_default); }
	static const char* GetIPAddrStr(char* buf, int buflen) { return ::ip4addr_ntoa_r(GetIPAddr(), buf, buflen); return buf; }
	static const char* GetNetmaskStr(char* buf, int buflen) { return ::ip4addr_ntoa_r(GetNetmask(), buf, buflen); return buf; }
	static const char* GetGatewayStr(char* buf, int buflen) { return ::ip4addr_ntoa_r(GetGateway(), buf, buflen); return buf; }
};

}

#endif
