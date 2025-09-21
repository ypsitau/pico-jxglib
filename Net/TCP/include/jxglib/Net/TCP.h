//==============================================================================
// jxglib/Net/TCP.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_TCP_H
#define PICO_JXGLIB_NET_TCP_H
#include "pico/stdlib.h"
#include "jxglib/Net.h"

namespace jxglib::Net::TCP {

//------------------------------------------------------------------------------
// Handler
//------------------------------------------------------------------------------
class Handler {
public:
	virtual void OnSent(size_t len) {}
	virtual void OnRecv(const uint8_t* data, size_t len) {}
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) {}
	virtual void OnDisconnect() {}
};

//------------------------------------------------------------------------------
// Common
//------------------------------------------------------------------------------
class Common {
protected:
	struct tcp_pcb *pcb_;
	Handler* pHandler_;
	Handler handlerDummy_;
public:
	Common();
protected:
	void SetPCB(struct tcp_pcb* pcb);
public:
	bool Send(const void* data, size_t len);
	void DiscardPCB();
public:
	void SetHandler(Handler& handler) { pHandler_ = &handler; }
	Handler& GetHandler() { return *pHandler_; }
private:
	static err_t callback_sent(void* arg, struct tcp_pcb* pcb, u16_t len);
	static err_t callback_recv(void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err);
	static void callback_err(void* arg, err_t err);
};

//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
class Server : public Common {
private:
	uint16_t port_;
	struct tcp_pcb *pcbListener_;
	struct tcp_pcb *pcbAccepter_;
public:
	Server(uint16_t port);
	Server(uint16_t port, Handler& handler) : Server(port) { SetHandler(handler); }
public:
	uint16_t GetPort() const { return port_; }
public:
	bool Start();
private:
	static err_t callback_accept(void* arg, struct tcp_pcb* pcb, err_t err);
};

//------------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------------

}

#endif
