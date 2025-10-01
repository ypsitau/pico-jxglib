//==============================================================================
// NetUtil.cpp
//==============================================================================
#include "jxglib/NetUtil.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/timeouts.h"
#include "pico/time.h"

namespace jxglib::Net {

//------------------------------------------------------------------------------
// DNS: Domain Name System
//------------------------------------------------------------------------------
bool DNS::GetHostByName(const char* hostname, ip_addr_t* addr, uint32_t msecTimeout)
{
	if (::ip4addr_aton(hostname, addr)) return true;
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
	Net::lwip_begin();
	int err = ::dns_gethostbyname(hostname, &addr_, callback_found, this);
	Net::lwip_end();
	completeFlag_ = (err == ERR_OK);
}

void DNS::callback_found(const char* hostname, const ip_addr_t* addr, void* arg)
{
	DNS* pDNS = reinterpret_cast<DNS*>(arg);
	if (addr) {
		pDNS->addr_ = *addr;
		pDNS->completeFlag_ = true;
	}
}

//------------------------------------------------------------------------------
// NTP: Network Time Protocol
//------------------------------------------------------------------------------
NTP::NTP() : completeFlag_(false), unixtime_(0), errorMsg_(""), addr_{ip_addr_any}
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
	const uint16_t port = 123; // NTP port
	if (ip_addr_isany(&addr_) && !Net::DNS().GetHostByName("pool.ntp.org", &addr_)) {
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
	udpClient_.Send(msg, sizeof(msg), addr_, port);
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

//------------------------------------------------------------------------------
// ICMP: Internet Control Message Protocol
//------------------------------------------------------------------------------
ICMP::ICMP() : pcb_(nullptr), completeFlag_(false), errorMsg_(""), addr_{ip_addr_any},
				seqNum_(0), identifier_(0x1234), timeSent_(0), timeRecv_(0)
{
	pcb_ = ::raw_new(IP_PROTO_ICMP);
	if (pcb_) {
		::raw_recv(pcb_, callback_recv, this);
	}
}

ICMP::~ICMP()
{
	if (pcb_) {
		::raw_remove(pcb_);
		pcb_ = nullptr;
	}
}

bool ICMP::Echo(const ip_addr_t& addr, uint32_t* pMsecEcho, uint32_t msecTimeout)
{
	if (!EchoAsync(addr)) return false;
	while (Tickable::GetCurrentTime() - timeSent_ < msecTimeout) {
		if (IsComplete()) {
			*pMsecEcho = timeRecv_ - timeSent_;
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

bool ICMP::EchoAsync(const ip_addr_t& addr)
{
	if (!pcb_) {
		errorMsg_ = "ICMP PCB not initialized";
		return false;
	}
	// DNS lookup for the hostname
	completeFlag_ = false;
	errorMsg_ = "";
	addr_ = addr;
	seqNum_++;
	// Allocate pbuf for ICMP echo request
	struct pbuf* pbuf = ::pbuf_alloc(PBUF_IP, sizeof(struct icmp_echo_hdr) + 32, PBUF_RAM);
	if (pbuf == nullptr) {
		errorMsg_ = "Failed to allocate pbuf";
		return false;
	}
	// Fill ICMP header
	struct icmp_echo_hdr* pEchoHdr = reinterpret_cast<struct icmp_echo_hdr*>(pbuf->payload);
	ICMPH_TYPE_SET(pEchoHdr, ICMP_ECHO);
	ICMPH_CODE_SET(pEchoHdr, 0);
	pEchoHdr->chksum = 0;
	pEchoHdr->id = identifier_;
	pEchoHdr->seqno = ::htons(seqNum_);
	// Fill data payload (simple pattern)
	uint8_t* data = reinterpret_cast<uint8_t*>(pbuf->payload) + sizeof(struct icmp_echo_hdr);
	for (int i = 0; i < 32; i++) data[i] = 0x20 + i;
	// Calculate checksum
	pEchoHdr->chksum = ::inet_chksum(pEchoHdr, pbuf->len);
	// Send the packet
	timeSent_ = Tickable::GetCurrentTime();
	err_t err = ::raw_sendto(pcb_, pbuf, &addr);
	::pbuf_free(pbuf);
	if (err != ERR_OK) {
		errorMsg_ = "Failed to send ICMP packet";
		return false;
	}
	return true;
}

uint8_t ICMP::OnRecv(struct raw_pcb* pcb, struct pbuf* pbuf, const ip_addr_t* addr)
{
	timeRecv_ = Tickable::GetCurrentTime();
	struct icmp_echo_hdr* pEchoHdr = reinterpret_cast<struct icmp_echo_hdr*>(reinterpret_cast<uint8_t*>(pbuf->payload) + IP_HLEN);
	if (pbuf->len >= sizeof(struct icmp_echo_hdr) && ip_addr_cmp(addr, &addr_) &&
			ICMPH_TYPE(pEchoHdr) == ICMP_ER && pEchoHdr->id == identifier_ && ::ntohs(pEchoHdr->seqno) == seqNum_) {
		completeFlag_ = true;
		errorMsg_ = ""; // Success
	}
	::pbuf_free(pbuf);
	return 1; // Packet consumed
}

uint8_t ICMP::callback_recv(void* arg, struct raw_pcb* pcb, struct pbuf* pbuf, const ip_addr_t* addr)
{
	if (!pbuf) return 0;
	return reinterpret_cast<ICMP*>(arg)->OnRecv(pcb, pbuf, addr);
}

}
