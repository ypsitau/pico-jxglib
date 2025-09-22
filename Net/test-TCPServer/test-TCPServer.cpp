#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"
#include "jxglib/Net/TCP.h"

using namespace jxglib;

class Handler : public Net::TCP::Handler {
public:
	virtual void OnRecv(const uint8_t* data, size_t len);
	virtual void OnConnect();
	virtual void OnDisconnect();
};

Net::WiFi wifi;
Handler tcpHandler;
Net::TCP::Server tcpServer(23);
Net::WiFi& ShellCmd_Net_GetWiFi() { return wifi; }

void Handler::OnRecv(const uint8_t* data, size_t len)
{
	::printf("Received %u bytes\n", len);
}

void Handler::OnConnect()
{
	::printf("Connected\n");
}

void Handler::OnDisconnect()
{
	::printf("Disconnected\n");
	tcpServer.Start();
}

ShellCmd(server, "start tcp echo server")
{
	tcpServer.SetHandler(tcpHandler);
	if (tcpServer.Start()) {
		terr.Printf("TCP server started on port %u\n", tcpServer.GetPort());
	}
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
