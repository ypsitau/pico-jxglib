//==============================================================================
// WiFi.cpp
//==============================================================================
#include "jxglib/WiFi.h"

namespace jxglib {

//------------------------------------------------------------------------------
// WiFi
//------------------------------------------------------------------------------
bool WiFi::InitAsStation()
{
	if (initializedFlag_) return true;
	if (::cyw43_arch_init_with_country(country_) != 0) return false;
	::cyw43_arch_enable_sta_mode();
	initializedFlag_ = true;
	return true;
}

bool WiFi::InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth)
{
	if (initializedFlag_) return true;
	if (::cyw43_arch_init_with_country(country_) != 0) return false;
	::cyw43_arch_enable_ap_mode(ssid, password, auth);
	initializedFlag_ = true;
	return true;
}

void WiFi::Deinit()
{
	::cyw43_arch_deinit();
	initializedFlag_ = false;
}

void WiFi::Poll()
{
	if (initializedFlag_) ::cyw43_arch_poll();
}

void WiFi::Scan(Printable& tout)
{
	auto result_cb = [](void* env, const cyw43_ev_scan_result_t* result) -> int {
		if (!result) return 0;
		Printable& tout = *reinterpret_cast<Printable*>(env);
		tout.Printf("ssid: %-32s rssi: %4d channel: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x auth_mode: %u\n",
			result->ssid, result->rssi, result->channel,
			result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
			result->auth_mode);
		return 0;
	};
	cyw43_wifi_scan_options_t scan_options = {0};
	if (!InitAsStation()) return;
	if (::cyw43_wifi_scan(&cyw43_state, &scan_options, &tout, result_cb) == 0) {
		while (::cyw43_wifi_scan_active(&cyw43_state)) {
			Tickable::Tick();
		}
	}
}

int WiFi::Connect(Printable& tout, const char* ssid, const char* password, uint32_t auth)
{
	if (!InitAsStation()) return false;
	::cyw43_arch_wifi_connect_async(ssid, password, auth);
	int status = 0;
	for (;;) {
		status = ::cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA);
		if (status == CYW43_LINK_DOWN) {
			// nothing to do
		} else if (status == CYW43_LINK_JOIN) {
			tout.Printf("Connected to access point, waiting for IP address...\n");
		} else if (status == CYW43_LINK_NOIP) {
			// nothing to do
		} else if (status == CYW43_LINK_UP) {
			tout.Printf("Connected to access point and got an IP address.\n");
			break;
		} else if (status == CYW43_LINK_FAIL) {
			tout.Printf("Connection failed\n");
			break;
		} else if (status == CYW43_LINK_NONET) {
			tout.Printf("No matching SSID found\n");
			break;
		} else if (status == CYW43_LINK_BADAUTH) {
			tout.Printf("Authentication failure\n");
			break;
		} else if (status == CYW43_LINK_DOWN) {
			tout.Printf("Link down\n");
			break;
		} else {
			tout.Printf("Unknown status: %d\n", status);
			break;
		}
		Tickable::Sleep(100);
	}
	return status;
}

//------------------------------------------------------------------------------
// WiFi::Polling
//------------------------------------------------------------------------------

}
