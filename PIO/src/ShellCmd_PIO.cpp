//==============================================================================
// ShellCmd_PIO.cpp
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/PIO.h"

using namespace jxglib;

void PrintStateMachineStatus(Printable& tout, pio_t pio, uint sm);

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
	for (uint iPIO = 0; iPIO < 2; ++iPIO) {
		pio_t pio = PIO::Block::get_instance(iPIO);
		for (uint smIdx = 0; smIdx < 4; ++smIdx) {
			PrintStateMachineStatus(tout, pio, smIdx);
		}
	}
	return Result::Success;
}

void PrintStateMachineStatus(Printable& tout, pio_t pio, uint smIdx)
{
	PIO::StateMachine sm(PIO::Program::None, pio, smIdx);
	tout.Printf("PIO%d:SM%d:\n", sm.pio.get_index(), smIdx);
}
