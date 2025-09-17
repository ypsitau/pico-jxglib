//==============================================================================
// jxglib/Net.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_H
#define PICO_JXGLIB_NET_H
#include "pico/stdlib.h"
#include "jxglib/WiFi.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"

namespace jxglib::Net {

//------------------------------------------------------------------------------
// DNS
//------------------------------------------------------------------------------
class DNS {
private:
	bool completeFlag_;
	ip_addr_t addr_;
public:
	DNS() : completeFlag_(false) {}
public:
	bool GetHostByName(const char* hostname, ip_addr_t* addr, uint32_t msecTimeout = 3000);
	void GetHostByNameAsync(const char* hostname);
	const ip_addr_t& GetAddr() const { return addr_; }
	bool IsComplete() { return completeFlag_; }
private:
	static void callback_found(const char*hostname, const ip_addr_t* addr, void* arg);
};

}

#endif
