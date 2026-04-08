//==============================================================================
// jxglib/UDP.h
//==============================================================================
#ifndef PICO_JXGLIB_UDP_H
#define PICO_JXGLIB_UDP_H
#include "pico/stdlib.h"
#include "jxglib/Net.h"

namespace jxglib::Net::UDP {

//------------------------------------------------------------------------------
// Handler
//------------------------------------------------------------------------------
class Handler {
public:
	virtual void OnRecv(const uint8_t* data, size_t len, const ip_addr_t& addr, uint16_t port) {}
};

//------------------------------------------------------------------------------
// Common
//------------------------------------------------------------------------------
class Common {
protected:
	struct udp_pcb *pcb_;
	Handler* pHandler_;
	Handler handlerDummy_;
public:
	Common();
	~Common();
public:
	bool Initialize(Handler& handler);
public:
	bool Send(const void* data, size_t len, const ip_addr_t& addr, uint16_t port);
	void DiscardPCB();
public:
	Handler& GetHandler() { return *pHandler_; }
private:
	static void callback_recv(void* arg, struct udp_pcb* pcb, struct pbuf* pbuf, const ip_addr_t* addr, u16_t port);
};

//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
using Server = Common;

//------------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------------
using Client = Common;

}

#endif
