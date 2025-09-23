#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Net/Telnet.h"

using namespace jxglib;

Net::WiFi wifi;

Net::WiFi& ShellCmd_Net_GetWiFi() { return wifi; }

Net::Telnet::Server telnetServer;
Net::Telnet::Stream telnetStream(telnetServer);

class BlinkLED : public Tickable {
private:
	bool value_;
	uint32_t msecStart_;
	uint32_t msecOn_;
	uint32_t msecOff_;
public:
	static BlinkLED Instance;
public:
	BlinkLED() : Tickable(0, Priority::Lowest), value_{false}, msecStart_{0}, msecOn_{500}, msecOff_{500} { Suspend(); }
public:
	void SetDurations(uint32_t msecOn, uint32_t msecOff) { msecOn_ = msecOn; msecOff_ = msecOff; }
	void Enable(bool enableFlag);
	virtual const char* GetTickableName() const override { return "BlinkLED"; }
	virtual void OnTick() override;
};

BlinkLED BlinkLED::Instance;

void BlinkLED::Enable(bool enableFlag)
{
	if (enableFlag) {
		value_ = true;
		Net::WiFi::PutLED(value_);
		msecStart_ = Tickable::GetCurrentTime();
		Resume();
	} else {
		Suspend();
		value_ = false;
		Net::WiFi::PutLED(value_);
	}
}

void BlinkLED::OnTick()
{
	uint32_t msecCur = Tickable::GetCurrentTime();
	if (Tickable::GetCurrentTime() - msecStart_ < (value_? msecOn_ : msecOff_)) return;
	value_ = !value_;
	Net::WiFi::PutLED(value_);
	msecStart_ = msecCur;
}

class Handler : public Net::Telnet::Handler {
public:
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) override {
		Printable& tout = Stdio::Instance;
		LABOPlatform::Instance.GetTerminal().AttachKeyboard(telnetStream.GetKeyboard());
		LABOPlatform::Instance.GetTerminal().AttachPrintable(telnetStream);
		tout.Printf("Telnet client connected: %s:%d\n", ipaddr_ntoa(&addr), port);
	}
	virtual void OnDisconnect() override {
		Printable& tout = Stdio::Instance;
		Shell::Logout();
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
		BlinkLED::Instance.SetDurations(msecOn, msecOff);
	}
	if (argc < 2) {
		// nothing to do
	} else if (::strcasecmp(argv[1], "on") == 0) {
		BlinkLED::Instance.Enable(true);
	} else if (::strcasecmp(argv[1], "off") == 0) {
		BlinkLED::Instance.Enable(false);
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
	jxglib_labo_init(true);
	while (true) {
		jxglib_tick();
	}
}
