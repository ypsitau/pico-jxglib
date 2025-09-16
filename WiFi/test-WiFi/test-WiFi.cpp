#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

using namespace jxglib;

class TCPCommon {
protected:
	struct tcp_pcb *pcb_;
public:
	TCPCommon();
protected:
	void SetPCB(struct tcp_pcb* pcb);
public:
	bool Send(const void* data, size_t len);
	void Close();
public:
	virtual void OnSent(size_t len);
	virtual void OnRecv(const uint8_t* data, size_t len);
	virtual void OnDisconnect() {}
private:
	static err_t callback_sent(void* arg, struct tcp_pcb* pcb, u16_t len);
	static err_t callback_recv(void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err);
	static err_t callback_poll(void* arg, struct tcp_pcb* pcb);
	static void callback_err(void* arg, err_t err);
};

TCPCommon::TCPCommon() : pcb_{nullptr}
{
}

void TCPCommon::SetPCB(struct tcp_pcb* pcb)
{
	pcb_ = pcb;
	::tcp_arg(pcb_, this);
	::tcp_sent(pcb_, callback_sent);
	::tcp_recv(pcb_, callback_recv);
	//::tcp_poll(pcb_, callback_poll, POLL_TIME_S * 2);
	::tcp_err(pcb_, callback_err);
}

bool TCPCommon::Send(const void* data, size_t len)
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

void TCPCommon::Close()
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

void TCPCommon::OnSent(size_t len)
{
	::printf("Sent %u bytes\n", len);
}

void TCPCommon::OnRecv(const uint8_t* data, size_t len)
{
	::printf("Received %u bytes\n", len);
}

err_t TCPCommon::callback_sent(void* arg, struct tcp_pcb* pcb, u16_t len)
{
	TCPCommon* pCommon = reinterpret_cast<TCPCommon*>(arg);
	pCommon->OnSent(len);
	return ERR_OK;
}

err_t TCPCommon::callback_recv(void* arg, struct tcp_pcb* pcb, struct pbuf* pbuf, err_t err)
{
	TCPCommon* pCommon = reinterpret_cast<TCPCommon*>(arg);
	if (!pbuf) {
		// connection closed
		pCommon->OnDisconnect();
		return ERR_OK;
	}
	uint8_t buff[128];
	for (int lenRest = pbuf->tot_len; lenRest > 0; ) {
		int lenCopied = ::pbuf_copy_partial(pbuf, buff, ChooseMin(pbuf->tot_len, sizeof(buff)), pbuf->tot_len - lenRest);
		pCommon->OnRecv(buff, lenCopied);
		lenRest -= lenCopied;
	}
	::tcp_recved(pcb, pbuf->tot_len);
	return ERR_OK;
}

err_t TCPCommon::callback_poll(void* arg, struct tcp_pcb* pcb)
{
	return ERR_OK;
}

void TCPCommon::callback_err(void* arg, err_t err)
{
}

class TCPServer : public TCPCommon {
private:
	uint16_t port_;
	struct tcp_pcb *pcbListener_;
public:
	TCPServer(uint16_t port);
public:
	bool Start();
public:
	void Close();
private:
	static err_t callback_accept(void* arg, struct tcp_pcb* pcb, err_t err);
public:
	virtual void OnDisconnect() override;
};

TCPServer::TCPServer(uint16_t port) : port_{port}, pcbListener_{nullptr}
{
}

bool TCPServer::Start()
{
	pcb_ = ::tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb_) return false;
	if (::tcp_bind(pcb_, nullptr, port_) != 0) return false;
	uint8_t backlog = 1;
	pcbListener_ = ::tcp_listen_with_backlog(pcb_, backlog);
	if (!pcbListener_) {
		::tcp_close(pcb_);
		pcb_ = nullptr;
		return false;
	}
	::tcp_arg(pcbListener_, this);
	::tcp_accept(pcbListener_, callback_accept);
	return true;
}

void TCPServer::Close()
{
	TCPCommon::Close();
	if (pcbListener_) {
		::tcp_arg(pcbListener_, nullptr);
		::tcp_accept(pcbListener_, nullptr);
		::tcp_close(pcbListener_);
		pcbListener_ = nullptr;
	}
}

err_t TCPServer::callback_accept(void* arg, struct tcp_pcb* pcb, err_t err)
{
	TCPServer* pServer = reinterpret_cast<TCPServer*>(arg);
	if (err != ERR_OK || !pcb) return ERR_VAL;
	::printf("Connected\n");
	pServer->SetPCB(pcb);
	return ERR_OK;
}

void TCPServer::OnDisconnect()
{
	::printf("Disconnected\n");
	Close();
	Start();
}

WiFi wifi;

TCPServer tcpServer(23);

WiFi& ShellCmd_WiFi_GetWiFi() { return wifi; }

ShellCmd(server, "start tcp echo server")
{
	//run_tcp_server_test();
	tcpServer.Start();
	return Result::Success;
}

int main()
{
	stdio_init_all();
	jxglib_labo_init(false);
	while (true) {
		jxglib_tick();
	}
}
