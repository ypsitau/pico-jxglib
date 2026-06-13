#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"

namespace jxglib::ShellCmd_Keyboard {

ShellCmd(keyboard, "lists available keyboards")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",			'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	for (const Keyboard* pKeyboard = Keyboard::GetListNodeHead(); pKeyboard; pKeyboard = pKeyboard->GetListNodeNext()) {
		tout.Printf("%s\n", pKeyboard->GetName());
	}
#if 0
	bool genericFlag = (::strcmp(GetName(), "keyboard") == 0);
	if (arg.GetBool("help") || (genericFlag && argc < 2)) {
		if (genericFlag) {
			tout.Printf("Usage: %s [OPTION]... [INPUT [COMMAND]...]\n", GetName());
		} else {
			tout.Printf("Usage: %s [OPTION]... [COMMAND]...\n", GetName());
		}
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf("  repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf("  sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf("  gpio-init            Prepare a GPIO for use with ADC by disabling all digital functions\n");
		tout.Printf("  read                 read ADC value\n");
		tout.Printf("Available inputs: %s\n", strAvailableInputs);
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	bool rawFlag = arg.GetBool("raw");
	const char* strIndex = nullptr;
	int nArgsToSkip = 0;
	if (genericFlag) {
		if (argc < 2) return Result::Error;
		strIndex = argv[1];
		nArgsToSkip = 2; // Skip "keyboard" and input index
	} else if (::strncmp(GetName(), "keyboard", 8) == 0) {
		strIndex = GetName() + 8; // Skip "keyboard" prefix
		nArgsToSkip = 1; // Skip "keyboard" command
	}
#endif
	return Result::Success;
}

}
