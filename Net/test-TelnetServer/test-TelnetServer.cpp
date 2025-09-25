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

#if defined(CYW43_WL_GPIO_LED_PIN)
static PeriodicToggle<Net::WiFi::Initialize, Net::WiFi::PutLED> blinkLED;
#else
static PeriodicGPIO::InitializeLED, GPIO::PutLED> blinkLED;
#endif

ShellCmd_Named(blink_led, "blink-led", "control blinking LED")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("duration",	'D',	"specifies the ON/OFF durations (ms)",	"ON[,OFF]"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [on|off]\n", GetName());
		arg.PrintHelp(tout);
		return Result::Success;
	}
	const char* value = nullptr;
	if (arg.GetString("duration", &value)) {
		Arg::EachNum each(value);
		int valueTbl[2];
		int n = each.GetAll(valueTbl, count_of(valueTbl));
		if (n <= 0) {
			terr.Printf("Invalid duration: %s\n", value);
			return Result::Error;
		}
		uint32_t msecOn = valueTbl[0];
		uint32_t msecOff = (n < 2)? msecOn : valueTbl[1];
		blinkLED.SetDurations(msecOn, msecOff);
	}
	if (argc < 2) {
		// nothing to do
	} else if (::strcasecmp(argv[1], "on") == 0) {
		blinkLED.Enable(true);
	} else if (::strcasecmp(argv[1], "off") == 0) {
		blinkLED.Enable(false);
	} else {
		terr.Printf("Invalid argument: %s\n", argv[1]);
		return Result::Error;
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
