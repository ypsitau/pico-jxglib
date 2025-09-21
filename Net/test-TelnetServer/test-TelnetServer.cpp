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

ShellCmd_Named(telnet_server, "telnet-server", "start telnet server")
{
	if (telnetServer.Start()) {
		terr.Printf("Telnet server started\n");
	}
	return Result::Success;
}

int main()
{
	stdio_init_all();
	jxglib_labo_init(false);
	//LABOPlatform::Instance.GetTerminal().AttachKeyboard(telnetStream.GetKeyboard()).AttachPrintable(telnetStream);
	while (true) {
		jxglib_tick();
	}
}
