#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Display/WS2812.h"

namespace jxglib::ShellCmd_Display_WS2812 {

ShellCmd_Named(display_ws2812, "display-ws2812", "initialize WS2812 display")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" setup  Set up WS2812 display with the given parameters:\n");
		terr.Printf("          {gpio:PIN [size:WxH]}\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	//Display::WS2812* pDisplay = nullptr;
	//pDisplay = new Display::WS2812(16, 16); // default size
	//pDisplay->Initialize(GPIO::Instance(9));
	return Result::Success;
}

}

