//==============================================================================
// ShellCmd_LED.cpp
//==============================================================================
#include "jxglib/Shell.h"
#if defined(CYW43_WL_GPIO_LED_PIN)
#include "jxglib/Net.h"
#endif

namespace jxglib::ShellCmd_LED {

#if defined(CYW43_WL_GPIO_LED_PIN)
static Flipper<Net::WiFi::Initialize, Net::WiFi::PutLED> flipperLED;
#else
static Flipper<GPIO::InitializeLED, GPIO::PutLED> flipperLED;
#endif

ShellCmd(led, "control LED")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		tout.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" on          turn on LED\n");
		tout.Printf(" off         turn off LED\n");
		tout.Printf(" flip:MS...  start blinking LED with specified durations (in msec)\n");
		tout.Printf("             e.g. flip:500,500 (500 msec ON, 500 msec OFF)\n");
		tout.Printf("             specifies * to end the blinking (eg. flip:100,500,100,*)\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	for (int iArg = 1; iArg < argc; iArg++) {
		const char* subcmd = argv[iArg];
		const char* value = nullptr;
		if (::strcasecmp(subcmd, "on") == 0) {
			flipperLED.Put(true);
		} else if (::strcasecmp(subcmd, "off") == 0) {
			flipperLED.Put(false);
		} else if (Arg::GetAssigned(subcmd, "flip", &value)) {
			Arg::EachNum each(value);
			uint32_t msecPeriodTbl[32];
			each.SetBlankValue(-1);
			int nPeriod = each.GetAll(msecPeriodTbl, count_of(msecPeriodTbl));
			if (nPeriod <= 0) {
				terr.Printf("Invalid duration: %s\n", value);
				return Result::Error;
			}
			flipperLED.StartFlip(msecPeriodTbl, nPeriod);
		} else {
			terr.Printf("Invalid sub-command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
