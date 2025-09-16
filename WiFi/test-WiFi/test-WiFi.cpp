#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#define TCP_PORT 23
#define DEBUG_printf printf
#define BUF_SIZE 2048
#define TEST_ITERATIONS 10
#define POLL_TIME_S 5

using namespace jxglib;

typedef struct TCP_SERVER_T_ {
	struct tcp_pcb *server_pcb;
	struct tcp_pcb *client_pcb;
} TCP_SERVER_T;

err_t callback_accept(void *arg, struct tcp_pcb *client_pcb, err_t err);
err_t callback_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
err_t callback_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t callback_poll(void *arg, struct tcp_pcb *tpcb);
void callback_err(void *arg, err_t err);

err_t tcp_server_close(void *arg)
{
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	err_t err = ERR_OK;
	if (state->client_pcb != NULL) {
		::tcp_arg(state->client_pcb, NULL);
		::tcp_poll(state->client_pcb, NULL, 0);
		::tcp_sent(state->client_pcb, NULL);
		::tcp_recv(state->client_pcb, NULL);
		::tcp_err(state->client_pcb, NULL);
		err = ::tcp_close(state->client_pcb);
		if (err != ERR_OK) {
			DEBUG_printf("close failed %d, calling abort\n", err);
			::tcp_abort(state->client_pcb);
			err = ERR_ABRT;
		}
		state->client_pcb = NULL;
	}
	if (state->server_pcb) {
		::tcp_arg(state->server_pcb, NULL);
		::tcp_close(state->server_pcb);
		state->server_pcb = NULL;
	}
	return err;
}

err_t tcp_server_result(void *arg, int status)
{
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if (status == 0) {
		DEBUG_printf("test success\n");
	} else {
		DEBUG_printf("test failed %d\n", status);
	}
	return tcp_server_close(arg);
}

err_t callback_accept(void *arg, struct tcp_pcb *client_pcb, err_t err)
{
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if (err != ERR_OK || client_pcb == NULL) {
		DEBUG_printf("Failure in accept\n");
		tcp_server_result(arg, err);
		return ERR_VAL;
	}
	DEBUG_printf("Client connected\n");

	state->client_pcb = client_pcb;
	::tcp_arg(client_pcb, state);
	::tcp_sent(client_pcb, callback_sent);
	::tcp_recv(client_pcb, callback_recv);
	::tcp_poll(client_pcb, callback_poll, POLL_TIME_S * 2);
	::tcp_err(client_pcb, callback_err);
	return ERR_OK;
}

err_t callback_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	return ERR_OK;
}

err_t callback_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *pbuf, err_t err)
{
	static bool firstFlag = true;
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
	if (!pbuf) return ERR_OK;
	cyw43_arch_lwip_check();
	::printf("callback_recv %d bytes\n", pbuf->tot_len);
	if (pbuf->tot_len > 0) {
		// Receive the buffer
		uint8_t buff[128];
		int len = ::pbuf_copy_partial(pbuf, buff, ChooseMin(pbuf->tot_len, sizeof(buff)), 0);
		Dump(buff, len);
		::tcp_recved(tpcb, pbuf->tot_len);
		static const uint8_t buffSend[] = {
			0xff, 0xfb, 0x01,
			0xff, 0xfb, 0x03,
			0xff, 0xfd, 0x18,
			0xff, 0xfd, 0x1f,
			0xff, 0xfd, 0x20,
		};
		if (buff[0] == 0xff && firstFlag) {
			// telnet negotiation
			::tcp_write(tpcb, buffSend, sizeof(buffSend), TCP_WRITE_FLAG_COPY);
		} else {
			// echo back
			::tcp_write(tpcb, buff, len, TCP_WRITE_FLAG_COPY);
		}
		firstFlag = false;
		//::tcp_write(tpcb, buffSend, sizeof(buffSend), TCP_WRITE_FLAG_COPY);
	}
	::pbuf_free(pbuf);
	return ERR_OK;
}

err_t callback_poll(void *arg, struct tcp_pcb *tpcb)
{
	return ERR_OK;
}

void callback_err(void *arg, err_t err)
{
}

void run_tcp_server_test(void)
{
	TCP_SERVER_T *state = reinterpret_cast<TCP_SERVER_T*>(calloc(1, sizeof(TCP_SERVER_T)));
	if (!state) {
		DEBUG_printf("failed to allocate state\n");
		return;
	}
	DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), TCP_PORT);

	struct tcp_pcb *pcb = ::tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		DEBUG_printf("failed to create pcb\n");
		return;
	}

	err_t err = ::tcp_bind(pcb, NULL, TCP_PORT);
	if (err) {
		DEBUG_printf("failed to bind to port %u\n", TCP_PORT);
		return;
	}

	state->server_pcb = ::tcp_listen_with_backlog(pcb, 1);
	if (!state->server_pcb) {
		DEBUG_printf("failed to listen\n");
		if (pcb) {
			::tcp_close(pcb);
		}
		return;
	}

	::tcp_arg(state->server_pcb, state);
	::tcp_accept(state->server_pcb, callback_accept);
	//free(state);
}

WiFi wifi;

WiFi& ShellCmd_WiFi_GetWiFi() { return wifi; }

ShellCmd(server, "start tcp echo server")
{
	run_tcp_server_test();
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
