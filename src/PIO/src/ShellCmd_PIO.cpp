//==============================================================================
// ShellCmd_PIO.cpp
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/PIO.h"

namespace jxglib::ShellCmd_PIO {

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
	for (int iPIO = 0; iPIO < 2; iPIO++) {
		pio_hw_t* pio = ::pio_get_instance(iPIO);
		for (int sm = 0; sm < 4; sm++) {
			tout.Printf("PIO%d SM%d: enabled=%d, pc=0x%02X, clkdiv=%.3f\n", iPIO, sm,
				iPIO,
				pio_sm_get_enabled(pio, sm),
				::pio_sm_get_pc(pio, sm),
				pio_sm_get_clkdiv(pio, sm));
		}
	}	
	return Result::Success;
}

}
