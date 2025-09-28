//==============================================================================
// jxglib/Net.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_H
#define PICO_JXGLIB_NET_H
#include "pico/stdlib.h"
#include "jxglib/Net/WiFi.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"

namespace jxglib::Net {

//------------------------------------------------------------------------------
// Transmitter
//------------------------------------------------------------------------------
class Transmitter {
public:
    static Transmitter Dummy;
public:
	virtual void OnSent(size_t len) {}
	virtual void OnRecv(const uint8_t* data, size_t len) {}
};

//------------------------------------------------------------------------------
// EventHandler
//------------------------------------------------------------------------------
class EventHandler {
public:
    static EventHandler Dummy;
public:
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) {}
	virtual void OnDisconnect() {}
};

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
inline void lwip_begin() { ::cyw43_arch_lwip_begin(); }
inline void lwip_end() { ::cyw43_arch_lwip_end(); }

}

#endif
