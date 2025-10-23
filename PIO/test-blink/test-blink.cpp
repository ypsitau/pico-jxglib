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
	.program("auto_blink")
		.pull()						// osr <- txfifo
		.out("y",		32)			// y[31:0] <- osr[31:0], osr[31:0] <- 0
	.wrap_target()
		.mov("x",		"y")
		.set("pins",	0b1)
	.L("loop1")
		.jmp("x--",		"loop1")	// Delay for (x + 1) cycles
		.mov("x",		"y")
		.set("pins",	0b0)
	.L("loop2")
		.jmp("x--",		"loop2")	// Delay for (x + 1) cycles
	.wrap()
	.end();
	PIO::StateMachine sm1;
	PIO::StateMachine sm2;
	sm1.set_program(program).reserve_set_pin(GPIO14).init().set_enabled();
	sm2.share_program(sm1).reserve_set_pin(GPIO15).init().set_enabled();
	sm1.put((::clock_get_hz(clk_sys) / (1 * 2)) - 3);
	sm2.put((::clock_get_hz(clk_sys) / (2 * 2)) - 3);
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
