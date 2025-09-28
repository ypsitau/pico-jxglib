//==============================================================================
// jxglib/Net/TCP.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_TCP_H
#define PICO_JXGLIB_NET_TCP_H
#include "pico/stdlib.h"
#include "jxglib/Net.h"

namespace jxglib::Net::TCP {

//------------------------------------------------------------------------------
// Common
//------------------------------------------------------------------------------
class Common {
protected:
	struct tcp_pcb *pcb_;
	Transmitter* pTransmitter_;
	EventHandler* pEventHandler_;
public:
	Common();
protected:
	void SetPCB(struct tcp_pcb* pcb);
public:
	bool Send(const void* data, size_t len, bool immediateFlag = false);
	bool Flush();
	void DiscardPCB();
public:
	void SetTransmitter(Transmitter& transmitter) { pTransmitter_ = &transmitter; }
	Transmitter& GetTransmitter() { return *pTransmitter_; }
public:
	void SetEventHandler(EventHandler& handler) { pEventHandler_ = &handler; }
	EventHandler& GetEventHandler() { return *pEventHandler_; }
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
public:
	uint16_t GetPort() const { return port_; }
public:
	bool Start();
	void Stop();
	bool IsRunning() const { return !!pcbAccepter_; }
private:
	static err_t callback_accept(void* arg, struct tcp_pcb* pcb, err_t err);
};

//------------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------------

}

#endif
