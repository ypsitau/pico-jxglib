#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Net/Telnet.h"

using namespace jxglib;

Net::Telnet::Server telnetServer;
Net::Telnet::Stream telnetStream(telnetServer);

class TelnetHandler : public Net::Telnet::Handler {
private:
	LABOPlatform& laboPlatform_;
public:
	TelnetHandler(LABOPlatform& laboPlatform) : laboPlatform_{laboPlatform} {}
public:
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) override;
	virtual void OnDisconnect() override;
};

void TelnetHandler::OnConnect(const ip_addr_t& addr, uint16_t port)
{
	Printable& tout = laboPlatform_.GetTerminal();
	tout.Printf("Telnet client connected: %s:%u\n", ::ipaddr_ntoa(&addr), port);
	Shell::Logout();
	laboPlatform_.SetTerminalInterface(telnetStream, telnetStream.GetKeyboard());
}

void TelnetHandler::OnDisconnect()
{
	laboPlatform_.RestoreTerminalInterface();
	Printable& tout = laboPlatform_.GetTerminal();
	tout.Printf("Telnet client disconnected\n");
}

TelnetHandler telnetHandler(LABOPlatform::Instance);

ShellCmd_Named(telnet_server, "telnet-server", "start telnet server")
{
	if (telnetServer.Start()) {
		terr.Printf("Telnet server started\n");
		telnetServer.SetHandler(telnetHandler);
	}
	return Result::Success;
}

int main()
{
	stdio_init_all();
	//jxglib_enable_startup_script(false);
	jxglib_labo_init(false);
	while (true) {
		jxglib_tick();
	}
}
