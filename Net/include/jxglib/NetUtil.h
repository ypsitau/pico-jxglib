//==============================================================================
// NetUtil.h
//==============================================================================
#ifndef PICO_JXGLIB_NETUTIL_H
#define PICO_JXGLIB_NETUTIL_H
#include "jxglib/Net.h"
#include "jxglib/Net/UDP.h"

// Forward declarations for lwIP types
struct raw_pcb;
struct pbuf;

namespace jxglib::Net {

//------------------------------------------------------------------------------
// DNS: Domain Name System
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

//------------------------------------------------------------------------------
// NTP: Network Time Protocol
//------------------------------------------------------------------------------
class NTP : public Net::UDP::Handler {
private:
	Net::UDP::Client udpClient_;
	bool completeFlag_;
	uint64_t unixtime_;
	const char* errorMsg_;
	ip_addr_t addr_;
public:
	NTP();
public:
	bool GetTime(DateTime& dt, uint32_t msecTimeout = 3000);
	bool GetTimeAsync();
	bool IsComplete() const { return completeFlag_; }
	const char* GetErrorMsg() const { return errorMsg_; }
public:
	virtual void OnRecv(const uint8_t* data, size_t len, const ip_addr_t& addr, uint16_t port) override;
};

//------------------------------------------------------------------------------
// ICMP: Internet Control Message Protocol
//------------------------------------------------------------------------------
class ICMP {
private:
	struct raw_pcb* pcb_;
	bool completeFlag_;
	const char* errorMsg_;
	ip_addr_t addr_;
	uint16_t seqNum_;
	uint16_t identifier_;
	uint32_t timeSent_;
	uint32_t timeRecv_;
public:
	ICMP();
	~ICMP();
public:
	bool Echo(const ip_addr_t& addr, uint32_t* pMsecEcho, uint32_t msecTimeout = 3000);
	bool EchoAsync(const ip_addr_t& addr);
	bool IsComplete() const { return completeFlag_; }
	const char* GetErrorMsg() const { return errorMsg_; }
private:
	bool SendEchoRequest(const ip_addr_t* destAddr);
	uint8_t OnRecv(struct raw_pcb* pcb, struct pbuf* pbuf, const ip_addr_t* addr);
private:
	static uint8_t callback_recv(void* arg, struct raw_pcb* pcb, struct pbuf* pbuf, const ip_addr_t* addr);
};

}

#endif
