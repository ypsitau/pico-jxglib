#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	PIO::Program program;
	program
	.program("gen_seq_number")
		.mov("x", "!null")
	.wrap_target()
		.mov("isr", "!x")
		.jmp("x--", "next")
	.L("next")
		.push()
	.end();
	PIO::StateMachine sm;
	sm.ClaimResource(program).init();
	sm.set_enabled();
	//-------------------------------------------------------------------------
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) {
		if (!sm.is_rx_fifo_empty()) ::printf("%08x\n", sm.get());
		Tickable::Tick();
	}
}
