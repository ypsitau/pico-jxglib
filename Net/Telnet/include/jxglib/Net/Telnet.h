//==============================================================================
// jxglib/Net/Telnet.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_TELNET_H
#define PICO_JXGLIB_NET_TELNET_H
#include "pico/stdlib.h"
#include "jxglib/WiFi.h"
#include "jxglib/Net/TCP.h"

namespace jxglib::Net::Telnet {

//------------------------------------------------------------------------------
// Handler
//------------------------------------------------------------------------------
class Handler {
public:
	virtual void OnSent(size_t len) {}
	virtual void OnRecv(const uint8_t* data, size_t len) {}
	virtual void OnConnect() {}
	virtual void OnDisconnect() {}
};

}

#endif
