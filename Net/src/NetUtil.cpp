//==============================================================================
// NetUtil.cpp
//==============================================================================
#include "jxglib/NetUtil.h"

namespace jxglib::Net {

//------------------------------------------------------------------------------
// DNS: Domain Name System
//------------------------------------------------------------------------------
bool DNS::GetHostByName(const char* hostname, ip_addr_t* addr, uint32_t msecTimeout)
{
	GetHostByNameAsync(hostname);
	uint32_t msecStart = Tickable::GetCurrentTime();
	while (Tickable::GetCurrentTime() - msecStart < msecTimeout) {
		if (IsComplete()) {
			*addr = GetAddr();
			return true;
		}
		if (Tickable::TickSub()) return false;
	}
	return false;
}

void DNS::GetHostByNameAsync(const char* hostname)
{
	completeFlag_ = false;
	::dns_gethostbyname(hostname, &addr_, callback_found, this);
}

void DNS::callback_found(const char*hostname, const ip_addr_t* addr, void* arg)
{
	DNS* pDNS = reinterpret_cast<DNS*>(arg);
	pDNS->completeFlag_ = !!addr;
	if (pDNS->completeFlag_) pDNS->addr_ = *addr;
}

//------------------------------------------------------------------------------
// NTP: Network Time Protocol
//------------------------------------------------------------------------------
NTP::NTP() : completeFlag_(false), unixtime_(0), errorMsg_("")
{
}

bool NTP::GetTime(DateTime& dt, uint32_t msecTimeout)
{
	if (!GetTimeAsync()) return false;
	uint32_t msecStart = Tickable::GetCurrentTime();
	while (Tickable::GetCurrentTime() - msecStart < msecTimeout) {
		if (IsComplete()) {
			dt.FromUnixTime(unixtime_);	
			return true;
		}
		if (Tickable::TickSub()) {
			errorMsg_ = "user interrupt";
			return false;
		}
	}
	errorMsg_ = "timeout";
	return false;
}

bool NTP::GetTimeAsync()
{
	ip_addr_t addr;
	const uint16_t port = 123; // NTP port
	if (!Net::DNS().GetHostByName("pool.ntp.org", &addr)) {
		errorMsg_ = "DNS lookup failed";
		return false;
	}
	completeFlag_ = false;
	unixtime_ = 0;
	if (!udpClient_.Initialize(*this)) {
		errorMsg_ = "UDP client initialization failed";
		return false;
	}
	uint8_t msg[48] = { 0x1b }; // NTP request message
	udpClient_.Send(msg, sizeof(msg), addr, port);
	return true;
}

void NTP::OnRecv(const uint8_t* data, size_t len, const ip_addr_t& addr, uint16_t port)
{
	const uint64_t unixtimeDelta = 2208988800;
	if (len < 44) return;
	uint64_t secsSince1900 =
			(static_cast<uint64_t>(data[40]) << 24) | (static_cast<uint64_t>(data[41]) << 16) |
			(static_cast<uint64_t>(data[42]) << 8) | (static_cast<uint64_t>(data[43]) << 0);
	unixtime_ = secsSince1900 - unixtimeDelta;
	completeFlag_ = true;
}	

}
