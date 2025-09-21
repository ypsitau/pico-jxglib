#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"
#include "jxglib/Net/Telnet.h"

using namespace jxglib;

WiFi wifi;

WiFi& ShellCmd_WiFi_GetWiFi() { return wifi; }

Net::Telnet::Server telnetServer;
Net::Telnet::Stream telnetStream(telnetServer);

class Handler : public Net::Telnet::Handler {
public:
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) override {
		Printable& tout = Stdio::Instance;
		tout.Printf("Telnet client connected: %s:%d\n", ipaddr_ntoa(&addr), port);
		LABOPlatform::Instance.GetTerminal().AttachKeyboard(telnetStream.GetKeyboard());
		LABOPlatform::Instance.GetTerminal().AttachPrintable(telnetStream);
	}
	virtual void OnDisconnect() override {
		Printable& tout = Stdio::Instance;
		tout.Printf("Telnet client disconnected\n");
	}
};

Handler handler;

ShellCmd_Named(telnet_server, "telnet-server", "start telnet server")
{
	if (telnetServer.Start()) {
		terr.Printf("Telnet server started\n");
		telnetServer.SetHandler(handler);
	}
	return Result::Success;
}

int main()
{
	stdio_init_all();
	//jxglib_enable_startup_script(false);
	jxglib_labo_init(false);
	Printable& tout = LABOPlatform::Instance.GetTerminal();
	while (true) {
		jxglib_tick();
	}
}
