//==============================================================================
// jxglib/Net/WiFi.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_WIFI_H
#define PICO_JXGLIB_NET_WIFI_H
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "jxglib/Common.h"

//------------------------------------------------------------------------------
// Net::WiFi
//------------------------------------------------------------------------------
namespace jxglib::Net {

class WiFi {
public:
	class ScanResult {
	private:
		cyw43_ev_scan_result_t entity_;
		std::unique_ptr<ScanResult> pNext_;
	public:
		ScanResult(const cyw43_ev_scan_result_t& entity) : entity_(entity), pNext_(nullptr) {}
	public:
		const cyw43_ev_scan_result_t& GetEntity() const { return entity_; }
		ScanResult* GetNext() { return pNext_.get(); }
		const ScanResult* GetNext() const { return pNext_.get(); }
		ScanResult* ReleaseNext() { return pNext_.release(); }
		void SetNext(ScanResult* pNext) { pNext_.reset(pNext); }
	public:
		bool IsIdentical(const cyw43_ev_scan_result_t& entity) const {
			return ::memcmp(entity_.bssid, entity.bssid, sizeof(entity_.bssid)) == 0;
		}
		int16_t GetRSSI() const { return entity_.rssi; }
	public:
		void Update(const cyw43_ev_scan_result_t& entity) { entity_ = entity; }
	public:
		void Print(Printable& tout) const;
		void PrintSingle(Printable& tout) const;
	};
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
	std::unique_ptr<ScanResult> pScanResult_;
public:
	static WiFi Instance;
public:
	WiFi();
public:
	WiFi& SetCouytrn(uint32_t country) { country_ = country; return *this; }
	const ConnectInfo& GetConnectInfo() const { return connectInfo_; }
	static void Initialize() { Instance.Initialize_(); }
	bool Initialize_();
	bool InitAsStation();
	bool InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth = CYW43_AUTH_WPA2_AES_PSK);
	void Deinitialize();
	void Poll();
	ScanResult* Scan();
	void AddScanResult(const cyw43_ev_scan_result_t& entity);
	int Connect(const char* ssid, const uint8_t* bssid, const char* password, uint32_t auth, uint32_t msecTimeout);
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
public:
	static void PutGPIO(int gpio, bool value) {
		if (::cyw43_is_initialized(&cyw43_state)) ::cyw43_arch_gpio_put(gpio, value);
	}
	static void InitLED() { ::cyw43_arch_init(); }
	static void PutLED(bool value) { PutGPIO(CYW43_WL_GPIO_LED_PIN, value); }
	static void PutVBUS(bool value) { PutGPIO(CYW43_WL_GPIO_VBUS_PIN, value); }
};

}

#endif
