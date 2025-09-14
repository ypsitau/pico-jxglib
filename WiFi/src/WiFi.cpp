//==============================================================================
// WiFi.cpp
//==============================================================================
#include "jxglib/WiFi.h"

//------------------------------------------------------------------------------
// WiFi
//------------------------------------------------------------------------------
namespace jxglib::WiFi {

static bool initializedFlag = false;

bool InitAsStation()
{
	if (initializedFlag) return true;
	if (::cyw43_arch_init() != 0) return false;
	::cyw43_arch_enable_sta_mode();
	initializedFlag = true;
	return true;
}

bool InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth)
{
	if (initializedFlag) return true;
	if (::cyw43_arch_init() != 0) return false;
	::cyw43_arch_enable_ap_mode(ssid, password, auth);
	initializedFlag = true;
	return true;
}

void Deinit()
{
	::cyw43_arch_deinit();
	initializedFlag = false;
}

void Poll()
{
	if (initializedFlag) ::cyw43_arch_poll();
}

class ScanEnv {
public:
	Printable& tout_;
public:
	ScanEnv(Printable& tout) : tout_(tout) {}
public:
	static int result_cb(void* env, const cyw43_ev_scan_result_t* result);
};

int ScanEnv::result_cb(void* env, const cyw43_ev_scan_result_t* result)
{
	return 0;
}

static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}

// Start a wifi scan
static void scan_worker_fn(async_context_t *context, async_at_time_worker_t *worker) {
    cyw43_wifi_scan_options_t scan_options = {0};
    int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
    if (err == 0) {
        bool *scan_started = (bool*)worker->user_data;
        *scan_started = true;
        printf("\nPerforming wifi scan\n");
    } else {
        printf("Failed to start scan: %d\n", err);
    }
}

void Scan(Printable& tout)
{
	if (!InitAsStation()) return;
    // Start a scan immediately
    bool scan_started = false;
    async_at_time_worker_t scan_worker = { .do_work = scan_worker_fn, .user_data = &scan_started };
    hard_assert(async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &scan_worker, 0));
    if (!cyw43_wifi_scan_active(&cyw43_state) && scan_started) {
        // Start a scan in 10s
        scan_started = false;
        hard_assert(async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &scan_worker, 10000));
    }
}

int Connect(Printable& tout, const char* ssid, const char* password, uint32_t auth)
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
class Polling : public Tickable {
private:
	static Polling polling_;
public:
	Polling() {}
public:
	virtual const char* GetTickableName() const override { return "WiFi::Polling"; }
	virtual void OnTick() override { Poll(); }
};

Polling Polling::polling_;

}
