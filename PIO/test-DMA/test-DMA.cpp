#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	const GPIO& gpio = GPIO2;
	uint nPins = 2;
	PIO::Program program;
	program
	.program("gen_seq_number")
	.L("loop").wrap_target()
		.mov("x", "~osr")
		.jmp("x--", "no_wrap_around")
		.mov("osr", "~x")
	.entry()
		.mov("isr", "null")
		.in("pins", nPins)			// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("do_report")	[1]
	.L("no_wrap_around")
		.mov("osr", "~x")			// increment osr (counter) by 1
		.mov("isr", "null")
		.in("pins", nPins)			// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("x!=y", "do_report")	// if pins state changed, report it
		.jmp("loop")		[2]
	.L("do_report")
		.push()						// push current pins state
		.in("osr", 32)				// push osr (counter)
		.mov("y", "x")				// save current pins state in y
	.wrap()
	.end();
	PIO::StateMachine sm;
	sm.config.set_in_shift_left(true, 32);	// shift left, autopush enabled, push threshold 32
	sm.config.set_in_pins(gpio);			// set input pins base
	sm.ClaimResource(program).init();
	sm.set_enabled();
	//-------------------------------------------------------------------------
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) {
		if (!sm.is_rx_fifo_empty()) {
			uint32_t data = sm.get();
			uint32_t timeStamp = sm.get();
			::printf("%08x: %08x\n", timeStamp, data);
		}
		Tickable::Tick();
	}
}
