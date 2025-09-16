//==============================================================================
// TCP.cpp
//==============================================================================
#include "jxglib/Net/TCP.h"

namespace jxglib::Net::TCP {

//------------------------------------------------------------------------------
// Common
//------------------------------------------------------------------------------
Common::Common() : pcb_{nullptr}, pHandler_{&handlerDummy_}
{
}

void Common::SetPCB(struct tcp_pcb* pcb)
{
	pcb_ = pcb;
	::tcp_arg(pcb_, this);
	::tcp_sent(pcb_, callback_sent);
	::tcp_recv(pcb_, callback_recv);
	//::tcp_poll(pcb_, callback_poll, POLL_TIME_S * 2);
	::tcp_err(pcb_, callback_err);
}

bool Common::Send(const void* data, size_t len)
{
	if (pcb_ && data && len > 0) {
		::cyw43_arch_lwip_check();
		err_t err = ::tcp_write(pcb_, data, len, TCP_WRITE_FLAG_COPY);
		if (err == ERR_OK) {
			::tcp_output(pcb_);
		} else {
			return false;
		}
	}
	return true;
}

void Common::DiscardPCB()
{
	if (pcb_) {
		::tcp_arg(pcb_, nullptr);
		::tcp_sent(pcb_, nullptr);
		::tcp_recv(pcb_, nullptr);
		::tcp_poll(pcb_, nullptr, 0);
		::tcp_err(pcb_, nullptr);
		::tcp_close(pcb_);
		pcb_ = nullptr;
	}
}

err_t Common::callback_sent(void* arg, struct tcp_pcb* pcb, u16_t len)
{
	Common* pCommon = reinterpret_cast<Common*>(arg);
	pCommon->GetHandler().OnSent(len);
	return ERR_OK;
}

err_t Common::callback_recv(void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err)
{
	Common* pCommon = reinterpret_cast<Common*>(arg);
	if (!pbuf) {
		// connection closed
		pCommon->DiscardPCB();
		pCommon->GetHandler().OnDisconnect();
		return ERR_OK;
	}
	uint8_t buff[128];
	for (int lenRest = pbuf->tot_len; lenRest > 0; ) {
		int lenCopied = ::pbuf_copy_partial(pbuf, buff, ChooseMin(pbuf->tot_len, sizeof(buff)), pbuf->tot_len - lenRest);
		pCommon->GetHandler().OnRecv(buff, lenCopied);
		lenRest -= lenCopied;
	}
	::tcp_recved(pcb, pbuf->tot_len);
	return ERR_OK;
}

err_t Common::callback_poll(void* arg, struct tcp_pcb* pcb)
{
	return ERR_OK;
}

void Common::callback_err(void* arg, err_t err)
{
}

//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
Server::Server(uint16_t port) : port_{port}, pcbListener_{nullptr}, pcbAccepter_{nullptr}
{
}

bool Server::Start()
{
	pcbListener_ = ::tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcbListener_) return false;
	if (::tcp_bind(pcbListener_, nullptr, port_) != 0) return false;
	uint8_t backlog = 1;
	pcbAccepter_ = ::tcp_listen_with_backlog(pcbListener_, backlog);
	if (!pcbAccepter_) {
		::tcp_close(pcbListener_);
		pcbListener_ = nullptr;
		return false;
	}
	::tcp_arg(pcbAccepter_, this);
	::tcp_accept(pcbAccepter_, callback_accept);
	return true;
}

err_t Server::callback_accept(void* arg, struct tcp_pcb* pcb, err_t err)
{
	Server* pServer = reinterpret_cast<Server*>(arg);
	if (err != ERR_OK || !pcb) return ERR_VAL;
	pServer->SetPCB(pcb);
	pServer->GetHandler().OnConnect();
	return ERR_OK;
}

//------------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------------

}
