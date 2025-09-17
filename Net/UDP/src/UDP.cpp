//==============================================================================
// UDP.cpp
//==============================================================================
#include "jxglib/Net/UDP.h"

namespace jxglib::Net::UDP {

//------------------------------------------------------------------------------
// Common
//------------------------------------------------------------------------------
Common::Common() : pcb_{nullptr}, pHandler_{&handlerDummy_}
{
}

bool Common::Initialize()
{
	pcb_ = ::udp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb_) return false;
	::udp_recv(pcb_, callback_recv, this);
	return true;
}

bool Common::Send(const void* data, size_t len, const ip_addr_t& addr, uint16_t port)
{
	::cyw43_arch_lwip_begin();
	struct pbuf *pbuf = ::pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
	::memcpy(pbuf->payload, data, len);
	::udp_sendto(pcb_, pbuf, &addr, port);
	::pbuf_free(pbuf);
	::cyw43_arch_lwip_end();
	return true;
}

void Common::DiscardPCB()
{
	if (pcb_) {
		::udp_recv(pcb_, nullptr, nullptr);
        ::udp_remove(pcb_);
		pcb_ = nullptr;
	}
}

void Common::callback_recv(void* arg, struct udp_pcb* pcb, struct pbuf* pbuf, const ip_addr_t* addr, u16_t port)
{
	Common* pCommon = reinterpret_cast<Common*>(arg);
	if (!pbuf) return;
	uint8_t buff[128];
	for (int lenRest = pbuf->tot_len; lenRest > 0; ) {
		int lenCopied = ::pbuf_copy_partial(pbuf, buff, ChooseMin(pbuf->tot_len, sizeof(buff)), pbuf->tot_len - lenRest);
		pCommon->GetHandler().OnRecv(buff, lenCopied, *addr, port);
		lenRest -= lenCopied;
	}
}

}
