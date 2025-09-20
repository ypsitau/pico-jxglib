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

//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
class Server : public TCP::Handler {
private:
	TCP::Server tcpServer_;
	Handler* pHandler_;
	Handler handlerDummy_;
public:
	Server(uint16_t port = 23);
	~Server();
public:
	void SetHandler(Handler& handler) { pHandler_ = &handler; }
	Handler& GetHandler() { return *pHandler_; }
public:
	bool Start(Handler* handler);
	void Stop();
	bool IsRunning() const;
	bool Send(const uint8_t* data, size_t len);
	bool Send(const char* str);
public:
	// virtual functions of TCP::Handler
	void OnSent(size_t len) override;
	void OnRecv(const uint8_t* data, size_t len) override;
	void OnConnect() override;
	void OnDisconnect() override;
};

}

#endif
