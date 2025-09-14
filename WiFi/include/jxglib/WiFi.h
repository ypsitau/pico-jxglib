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
namespace jxglib::WiFi {

bool InitAsStation();
bool InitAsAccessPoint(const char* ssid, const char* password, uint32_t auth = CYW43_AUTH_WPA2_AES_PSK);
void Deinit();
void Poll();
void Scan(Printable& tout);
int Connect(Printable& tout, const char* ssid, const char* password, uint32_t auth);

}

#endif
