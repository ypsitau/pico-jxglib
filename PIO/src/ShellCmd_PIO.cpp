//==============================================================================
// ShellCmd_PIO.cpp
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/PIO.h"

using namespace jxglib;

ShellCmd(pio, "controls PIO pins")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help", 'h', "prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		tout.Printf("Usage: %s [OPTION]... [PIN [COMMAND]...]\n", GetName());
		arg.PrintHelp(tout);
		tout.Printf("Sub Commands:\n");
		return Result::Success;
	}
	return Result::Success;
}
