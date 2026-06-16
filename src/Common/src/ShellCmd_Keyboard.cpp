#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/KeyboardTest.h"
namespace jxglib::ShellCmd_Keyboard {

bool ProcessKeyboard(Printable& terr, Printable& tout, Keyboard& keyboard, int argc, char* argv[]);

ShellCmd(keyboard, "lists available keyboards")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",			'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [INDEX [SUB COMMAND]...]\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf(" repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf(" test-GetKeyData      test Keyboard::GetKeyDataNB()\n");
		tout.Printf(" test-GetKeyCode      test Keyboard::GetKeyCodeNB()\n");
		tout.Printf(" test-SenseKeyData    test Keyboard::SenseKeyData()\n");
		tout.Printf(" test-SenseKeyCode    test Keyboard::SenseKeyCode()\n");
		return Result::Success;
	}
	if (argc < 2) {
		int iKeyboard = 0;
		for (const Keyboard* pKeyboard = Keyboard::GetListNodeHead(); pKeyboard; pKeyboard = pKeyboard->GetListNodeNext(), iKeyboard++) {
			tout.Printf("%d: %s\n", iKeyboard, pKeyboard->GetName());
		}
		return Result::Success;
	}
	char* endp;
	int index = static_cast<int>(std::strtol(argv[1], &endp, 10));
	if (*endp != '\0' || index < 0) {
		terr.Printf("invalid keyboard index: %s\n", argv[1]);
		return Result::Error;
	}
	Keyboard& keyboard = Keyboard::N(index);
	if (keyboard.IsDumb()) {
		terr.Printf("keyboard %d is not available\n", index);
		return Result::Error;
	}
	return ProcessKeyboard(terr, tout, keyboard, argc - 2, argv + 2)? Result::Success : Result::Error;
}

bool ProcessKeyboard(Printable& terr, Printable& tout, Keyboard& keyboard, int argc, char* argv[])
{
	Shell::Arg::EachSubcmd each(argv[0], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return false;
	}
	while (const char* subcmd = each.Next()) {
		const char* value = nullptr;
		if (Shell::Arg::GetAssigned(subcmd, "test-GetKeyData", &value)) {
			KeyboardTest::GetKeyDataNB(terr, keyboard);
		} else if (Shell::Arg::GetAssigned(subcmd, "test-GetKeyCode", &value)) {
			KeyboardTest::GetKeyCodeNB(terr, keyboard);
		} else if (Shell::Arg::GetAssigned(subcmd, "test-SenseKeyData", &value)) {
			KeyboardTest::SenseKeyData(terr, keyboard);
		} else if (Shell::Arg::GetAssigned(subcmd, "test-SenseKeyCode", &value)) {
			KeyboardTest::SenseKeyCode(terr, keyboard);
		} else {
			terr.Printf("unknown subcommand: %s\n", subcmd);
			return false;
		}
	}
	return true;
}

}
