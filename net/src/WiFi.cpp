//==============================================================================
// WiFi.cpp
//==============================================================================
#include "jxglib/WiFi.h"

namespace jxglib::Net {

//------------------------------------------------------------------------------
// Net::WiFi
//------------------------------------------------------------------------------
WiFi::WiFi(uint32_t country) : country_(country), initializedFlag_(false), polling_(*this),
		connectInfo_{"", CYW43_AUTH_OPEN}
{
}

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

const WiFi::ScanResult* WiFi::Scan()
{
	auto result_cb = [](void* env, const cyw43_ev_scan_result_t* result) -> int {
		WiFi* pWiFi = reinterpret_cast<WiFi*>(env);
		if (result) pWiFi->AddScanResult(*result);
		return 0;
	};
	cyw43_wifi_scan_options_t scan_options = {0};
	pScanResult_.reset();
	if (!InitAsStation()) return nullptr;
	if (::cyw43_wifi_scan(&cyw43_state, &scan_options, this, result_cb) == 0) {
		while (::cyw43_wifi_scan_active(&cyw43_state)) Tickable::Tick();
	}
	return pScanResult_.get();
}

void WiFi::AddScanResult(const cyw43_ev_scan_result_t& entity)
{
	if (!pScanResult_) {
		pScanResult_.reset(new ScanResult(entity));
		return;
	}
	for (ScanResult* pScanResult = pScanResult_.get(); pScanResult; pScanResult = pScanResult->GetNext()) {
		if (pScanResult->IsIdentical(entity)) {
			//if (entity.rssi > pScanResult->GetRSSI()) pScanResult->Update(entity);
			return;
		}
	}
	std::unique_ptr<ScanResult> pScanResultNew(new ScanResult(entity));
	if (entity.rssi > pScanResult_->GetRSSI()) {
		pScanResultNew->SetNext(pScanResult_.release());
		pScanResult_.reset(pScanResultNew.release());
		return;
	}
	ScanResult* pScanResultIter = pScanResult_.get();
	for (; pScanResultIter->GetNext(); pScanResultIter = pScanResultIter->GetNext()) {
		if (entity.rssi > pScanResultIter->GetNext()->GetRSSI()) {
			pScanResultNew->SetNext(pScanResultIter->ReleaseNext());
			pScanResultIter->SetNext(pScanResultNew.release());
			return;
		}
	}
	pScanResultIter->SetNext(pScanResultNew.release());
}

int WiFi::Connect(const char* ssid, const uint8_t* bssid, const char* password, uint32_t auth)
{
	int linkStat = CYW43_LINK_FAIL;
	if (!InitAsStation()) return linkStat;
	if (::cyw43_arch_wifi_connect_bssid_async(ssid, bssid, password, auth) != 0) return linkStat;
	::snprintf(connectInfo_.ssid, sizeof(connectInfo_.ssid), "%s", ssid);
	connectInfo_.auth = auth;
	for (;;) {
		linkStat = ::cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
		if (linkStat == CYW43_LINK_DOWN) {
			// nothing to do
		} else if (linkStat == CYW43_LINK_JOIN) {
			// nothing to do
		} else if (linkStat == CYW43_LINK_NOIP) {
			// nothing to do
		} else if (linkStat == CYW43_LINK_UP) {
			break;
		} else if (linkStat == CYW43_LINK_FAIL) {
			break;
		} else if (linkStat == CYW43_LINK_NONET) {
			break;
		} else if (linkStat == CYW43_LINK_BADAUTH) {
			break;
		} else {
			break;
		}
		if (Tickable::Sleep(100)) break;
	}
	return linkStat;
}

WiFi& WiFi::Configure(const ip4_addr_t& addr, const ip4_addr_t& netmask, const ip4_addr_t& gateway)
{
	::netif_set_addr(cyw43_state.netif, &addr, &netmask, &gateway);
	return *this;
}

void WiFi::Disconnect()
{
	::cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
}

void WiFi::PrintStatus(Printable& tout) const
{
	int linkStat = ::cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
	if (linkStat == CYW43_LINK_UP) {
		char strAddr[16], strNetmask[16], strGateway[16];
		tout.Printf("Connected ssid:'%s' auth:%s addr:%s netmask:%s gateway:%s\n",
			connectInfo_.ssid[0]? connectInfo_.ssid : "(none)",
			AuthToString(connectInfo_.auth),
			::ip4addr_ntoa_r(&GetAddr(), strAddr, sizeof(strAddr)),
			::ip4addr_ntoa_r(&GetNetmask(), strNetmask, sizeof(strNetmask)),
			::ip4addr_ntoa_r(&GetGateway(), strGateway, sizeof(strGateway)));
	} else {
		tout.Printf("Not connected\n");
	}
}

bool WiFi::StringToAuth(const char* str, uint32_t* pAuth)
{
	if (::strcasecmp(str, "wpa") == 0) {
		*pAuth = CYW43_AUTH_WPA_TKIP_PSK;
	} else if (::strcasecmp(str, "wpa2") == 0) {
		*pAuth = CYW43_AUTH_WPA2_AES_PSK;
	} else if (::strcasecmp(str, "wpa3") == 0) {
		*pAuth = CYW43_AUTH_WPA3_SAE_AES_PSK;
	} else if (::strcasecmp(str, "wpa/wpa2") == 0 || ::strcasecmp(str, "wpa+wpa2") == 0) {
		*pAuth = CYW43_AUTH_WPA2_MIXED_PSK;
	} else if (::strcasecmp(str, "wpa2/wpa3") == 0 || ::strcasecmp(str, "wpa2+wpa3") == 0) {
		*pAuth = CYW43_AUTH_WPA3_WPA2_AES_PSK;
	} else if (::strcasecmp(str, "open") == 0) {
		*pAuth = CYW43_AUTH_OPEN;
	} else {
		return false;
	}
	return true;
}

const char* WiFi::AuthToString(uint32_t auth)
{
	switch (auth) {
	case CYW43_AUTH_WPA_TKIP_PSK: return "wpa";
	case CYW43_AUTH_WPA2_AES_PSK: return "wpa2";
	case CYW43_AUTH_WPA3_SAE_AES_PSK: return "wpa3";
	case CYW43_AUTH_WPA2_MIXED_PSK: return "wpa/wpa2";
	case CYW43_AUTH_WPA3_WPA2_AES_PSK: return "wpa2/wpa3";
	case CYW43_AUTH_OPEN: return "open";
	default: return "unknown";
	}
}

//------------------------------------------------------------------------------
// Net::WiFi::ScanResult
//------------------------------------------------------------------------------
void WiFi::ScanResult::Print(Printable& tout) const
{
	tout.Printf("ssid:%-32s rssi:%4d channel:%3d mac:%02x:%02x:%02x:%02x:%02x:%02x security:%u\n",
		entity_.ssid, entity_.rssi, entity_.channel,
		entity_.bssid[0], entity_.bssid[1], entity_.bssid[2], entity_.bssid[3], entity_.bssid[4], entity_.bssid[5],
		entity_.auth_mode);
}

//------------------------------------------------------------------------------
// Net::WiFi::Polling
//------------------------------------------------------------------------------

}
