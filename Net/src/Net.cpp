//==============================================================================
// Net.cpp
//==============================================================================
#include "jxglib/Net.h"

namespace jxglib::Net {

//------------------------------------------------------------------------------
// DNS
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
	DNS* pDNS = static_cast<DNS*>(arg);
	if (pDNS->completeFlag_ = (addr != nullptr)) {
		pDNS->addr_ = *addr;
	}
}

}
