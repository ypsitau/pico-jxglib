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
		//LABOPlatform::Instance.GetTerminal().AttachKeyboard(telnetStream.GetKeyboard()).AttachPrintable(telnetStream);
	}
	return Result::Success;
}

int main()
{
	stdio_init_all();
	jxglib_labo_init(false);
	Printable& tout = LABOPlatform::Instance.GetTerminal();
	while (true) {
		uint8_t buff[64];
		int len = telnetStream.Read(buff, sizeof(buff));
		if (len > 0) tout.Dump(buff, len);
		//Keyboard& keyboard = telnetStream.GetKeyboard();
		//uint8_t keyCode;
		//if (keyboard.GetKeyCode(&keyCode)) {
		//	telnetStream.Printf("%d\n", keyCode);
		//}
		jxglib_tick();
	}
}
