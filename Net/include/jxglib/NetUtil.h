//==============================================================================
// NetUtil.h
//==============================================================================
#ifndef PICO_JXGLIB_NETUTIL_H
#define PICO_JXGLIB_NETUTIL_H
#include "jxglib/Net.h"
#include "jxglib/Net/UDP.h"

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
	void OnRecv(const uint8_t* data, size_t len, const ip_addr_t& addr, uint16_t port) override;
};

//------------------------------------------------------------------------------
// ICMP: Internet Control Message Protocol
//------------------------------------------------------------------------------
class ICMP : public Net::UDP::Handler {
private:
	Net::UDP::Client udpClient_;
	bool completeFlag_;
	const char* errorMsg_;
	ip_addr_t addr_;
public:
	ICMP();
public:
	bool Echo(const char* hostName, uint32_t msecTimeout = 3000);
	bool EchoAsync(const char* hostName);
	bool IsComplete() const { return completeFlag_; }
	const char* GetErrorMsg() const { return errorMsg_; }
public:
	void OnRecv(const uint8_t* data, size_t len, const ip_addr_t& addr, uint16_t port) override;
};

}

#endif
