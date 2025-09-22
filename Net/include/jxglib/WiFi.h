//==============================================================================
// jxglib/WiFi.h
//==============================================================================
#ifndef PICO_JXGLIB_WIFI_H
#define PICO_JXGLIB_WIFI_H
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "jxglib/Common.h"

//------------------------------------------------------------------------------
// Net::WiFi
//------------------------------------------------------------------------------
namespace jxglib::Net {

class WiFi {
public:
	class Polling : public Tickable {
	private:
		WiFi& wifi_;
	public:
		Polling(WiFi& wifi) : wifi_(wifi) {}
	public:
		virtual const char* GetTickableName() const override { return "Net::WiFi::Polling"; }
		virtual void OnTick() override { wifi_.Poll(); }
	};
private:
	uint32_t country_;
	bool initializedFlag_;
	Polling polling_;
	struct ConnectInfo {
		char ssid[33];
		uint32_t auth;
	} connectInfo_;
public:
	WiFi(uint32_t country = PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE);
public:
	const ConnectInfo& GetConnectInfo() const { return connectInfo_; }
	bool InitAsStation();
	bool InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth = CYW43_AUTH_WPA2_AES_PSK);
	void Deinit();
	void Poll();
	void Scan(Printable& tout);
	int Connect(const char* ssid, const uint8_t* bssid, const char* password, uint32_t auth);
	WiFi& Configure(const ip4_addr_t& addr, const ip4_addr_t& netmask, const ip4_addr_t& gateway);
	void Disconnect();
public:
	void PrintStatus(Printable& tout) const;
	static bool StringToAuth(const char* str, uint32_t* pAuth);
	static const char* AuthToString(uint32_t auth);
public:
	static const ip_addr_t& GetAddr() { return *netif_ip_addr4(netif_default); }
	static const ip_addr_t& GetNetmask() { return *netif_ip_netmask4(netif_default); }
	static const ip_addr_t& GetGateway() { return *netif_ip_gw4(netif_default); }
};

}

#endif
