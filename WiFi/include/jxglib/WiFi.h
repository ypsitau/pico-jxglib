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
	Polling polling_;
public:
	WiFi(uint32_t country = PICO_CYW43_ARCH_DEFAULT_COUNTRY_CODE) : country_(country), initializedFlag_(false), polling_(*this) {}
public:
	bool InitAsStation();
	bool InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth = CYW43_AUTH_WPA2_AES_PSK);
	void Deinit();
	void Poll();
	void Scan(Printable& tout);
	int Connect(Printable& tout, const char* ssid, const uint8_t* bssid, const char* password, uint32_t auth);
};

}

#endif
