#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Device/WS2812.h"

jxglib::Device::WS2812& ShellCmd_Device_WS2812_GetWS2812();

namespace jxglib::ShellCmd_Device_WS2812 {

uint pinDIN = GPIO::InvalidPin;
int brightness = 256;  // brightness ratio (0 - 256)

ShellCmd(ws2812, "controls WS2812")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("brightness",	'b',	"set brightness (0.0 - 1.0)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	bool printHelpFlag = (argc < 2);
	const char* value = nullptr;
	if (arg.GetString("brightness", &value)) {
		char* endptr = nullptr;
		float num = ::strtof(value, &endptr);
		if (endptr == value || num < 0.0f || num > 1.0f) {
			terr.Printf("invalid brightness ratio: %s\n", value);
			return Result::Error;
		}
		brightness = static_cast<int>(num * 256.0f);
		printHelpFlag = false;
	}
	if (arg.GetBool("help") || printHelpFlag) {
		terr.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" sleep:MSEC           sleep for specified milliseconds\n");
		terr.Printf(" repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		terr.Printf(" setup                setup a WS2812 device with the given parameters: {din:PIN}\n");
		terr.Printf(" put:COLOR            put the color (COLOR format: #RRGGBB or color name)\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (::strcasecmp(subcmd, "setup") == 0) {
			for (const Arg::Subcmd* pSubcmdChild = pSubcmd->GetChild(); pSubcmdChild; pSubcmdChild = pSubcmdChild->GetNext()) {
				const char* subcmd = pSubcmdChild->GetProc();
				if (Arg::GetAssigned(subcmd, "din", &value)) {
					int num = ::strtol(value, nullptr, 0);
					if (num < 0 || num >= GPIO::NumPins) {
						terr.Printf("invalid GPIO number: %s\n", value);
						return Result::Error;
					}
					pinDIN = static_cast<uint>(num);
				} else {
					terr.Printf("unknown sub command: %s\n", subcmd);
					return Result::Error;
				}
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "put", &value)) {
			Device::WS2812& ws2812 = ShellCmd_Device_WS2812_GetWS2812();
			if (!ws2812.IsRunning()) {
				if (pinDIN == GPIO::InvalidPin) {
					terr.Printf("specify DIN pin number by setup subcommand\n");
					return Result::Error;
				}
				ws2812.Run(GPIO::Instance(pinDIN));
			}
			Color c;
			if (!c.Parse(value)) {
				terr.Printf("invalid color: %s\n", value);
				return Result::Error;
			}
			ws2812.Put(
				static_cast<uint8_t>(static_cast<int>(c.r) * brightness / 256),
				static_cast<uint8_t>(static_cast<int>(c.g) * brightness / 256),
				static_cast<uint8_t>(static_cast<int>(c.b) * brightness / 256));
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
