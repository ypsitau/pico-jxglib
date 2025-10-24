#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Device/WS2812.h"

namespace jxglib::ShellCmd_Device_WS2812 {

Device::WS2812 ws2812;
uint pin = static_cast<uint>(-1);
bool initializedFlag = false;

ShellCmd(ws2812, "controls WS2812")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("pin",		'p',	"specify the GPIO pin"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf(" repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf(" put:COLOR            put the color (COLOR format: #RRGGBB or color name)\n");
		return Result::Success;
	}
	const char* value = nullptr;
	if (arg.GetString("pin", &value)) {
		int num = ::strtol(value, nullptr, 0);
		if (num < 0 || num >= GPIO::NumPins) {
			terr.Printf("invalid GPIO pin number: %s\n", value);
			return Result::Error;
		}
		pin = static_cast<uint>(num);
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const char* subcmd = each.Next()) {
		if (Shell::Arg::GetAssigned(subcmd, "put", &value)) {
			if (!initializedFlag) {
				if (pin == static_cast<uint>(-1)) {
					terr.Printf("specify GPIO pin number by -p option\n");
					return Result::Error;
				}
				ws2812.Initialize(GPIO::Instance(pin));
				initializedFlag = true;
			}
			Color c;
			if (!c.Parse(value)) {
				terr.Printf("invalid color: %s\n", value);
				return Result::Error;
			}
			ws2812.Put(c);
		} else if (Shell::Arg::GetAssigned(subcmd, "sleep", &value)) {
			if (!value) {
				terr.Printf("specify a sleep duration in milliseconds\n");
				return false;
			}
			int msec = ::strtol(value, nullptr, 0);
			if (msec <= 0) {
				terr.Printf("Invalid sleep duration: %s\n", value);
				return false;
			}
			Tickable::Sleep(msec);
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

}
