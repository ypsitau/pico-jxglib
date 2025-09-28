//==============================================================================
// TCP.cpp
//==============================================================================
#include "jxglib/Net/TCP.h"

namespace jxglib::Net::TCP {

//------------------------------------------------------------------------------
// Base
//------------------------------------------------------------------------------
Base::Base() : pcb_{nullptr}, pTransmitter_{&Transmitter::Dummy}, pEventHandler_{&EventHandler::Dummy}
{
}

void Base::SetPCB(struct tcp_pcb* pcb)
{
	pcb_ = pcb;
	::tcp_arg(pcb_, this);
	::tcp_sent(pcb_, callback_sent);
	::tcp_recv(pcb_, callback_recv);
	::tcp_err(pcb_, callback_err);
}

bool Base::Send(const void* data, size_t len, bool immediateFlag)
{
	bool rtn = true;
	Net::lwip_begin();
	rtn = (::tcp_write(pcb_, data, len, TCP_WRITE_FLAG_COPY) == ERR_OK);
	if (rtn && immediateFlag) rtn = (::tcp_output(pcb_) == ERR_OK);
	Net::lwip_end();
	return rtn;
}

bool Base::Flush()
{
	Net::lwip_begin();
	err_t err = ::tcp_output(pcb_);
	Net::lwip_end();
	return err == ERR_OK;
}

void Base::DiscardPCB()
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

err_t Base::callback_sent(void* arg, struct tcp_pcb* pcb, u16_t len)
{
	Base* pCommon = reinterpret_cast<Base*>(arg);
	pCommon->GetTransmitter().OnSent(len);
	return ERR_OK;
}

err_t Base::callback_recv(void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err)
{
	Base* pCommon = reinterpret_cast<Base*>(arg);
	if (!pbuf) {
		// connection closed
		pCommon->DiscardPCB();
		pCommon->GetEventHandler().OnDisconnect();
		return ERR_OK;
	}
	uint8_t buff[512];
	for (int lenRest = pbuf->tot_len; lenRest > 0; ) {
		int lenCopied = ::pbuf_copy_partial(pbuf, buff, ChooseMin(pbuf->tot_len, sizeof(buff)), pbuf->tot_len - lenRest);
		pCommon->GetTransmitter().OnRecv(buff, lenCopied);
		lenRest -= lenCopied;
	}
	::tcp_recved(pcb, pbuf->tot_len);
	::pbuf_free(pbuf);
	return ERR_OK;
}

void Base::callback_err(void* arg, err_t err)
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

void Server::Stop()
{
	if (pcbAccepter_) {
		::tcp_arg(pcbAccepter_, nullptr);
		::tcp_accept(pcbAccepter_, nullptr);
		::tcp_close(pcbAccepter_);
		pcbAccepter_ = nullptr;
	}
	if (pcbListener_) {
		::tcp_arg(pcbListener_, nullptr);
		::tcp_close(pcbListener_);
		pcbListener_ = nullptr;
	}
}

err_t Server::callback_accept(void* arg, struct tcp_pcb* pcb, err_t err)
{
	Server* pServer = reinterpret_cast<Server*>(arg);
	if (err != ERR_OK || !pcb) return ERR_VAL;
	pServer->SetPCB(pcb);
	pServer->GetEventHandler().OnConnect(pcb->remote_ip, pcb->remote_port);
	return ERR_OK;
}

//------------------------------------------------------------------------------
// Client
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
Stream::Stream(Base& base) : base_(base)
{
	base_.SetTransmitter(*this);
}

int Stream::Read(void* buff, int bytesBuff)
{
	int bytesRead = 0;
	while (bytesRead < bytesBuff) {
		int bytes = buffRecv_.ReadBuff(static_cast<uint8_t*>(buff) + bytesRead, bytesBuff - bytesRead);
		if (bytes <= 0) break; // no more data available
		bytesRead += bytes;
		Tickable::TickSub();
	}
	return bytesRead;
}

int Stream::Write(const void* buff, int bytesBuff)
{
	//::printf("Write %dbytes\n", bytesBuff);
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += base_.Send(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::TickSub();
	}
	return bytesWritten;
}

bool Stream::Flush()
{
	base_.Flush();
	return true;
}

void Stream::OnSent(size_t len)
{
	// nothing to do
}

void Stream::OnRecv(const uint8_t* data, size_t len)
{
	//::printf("OnRecv(len=%d)\n", len);
	buffRecv_.WriteBuff(data, len);
}

}
