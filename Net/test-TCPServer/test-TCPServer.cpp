#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/WiFi.h"
#include "jxglib/Net/TCP.h"

using namespace jxglib;

WiFi wifi;

Net::TCP::Server tcpServer(23);

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
