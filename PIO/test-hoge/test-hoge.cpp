#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	const GPIO& gpioLED = GPIO15;
	const uint freq = 2;
	::stdio_init_all();
	PIO::Program blink_program;
	blink_program
	.program("blink")
		.pull().block()
		.out("y",		32)			// y <- osr[31:0], osr >>= 32
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
	PIO::StateMachine sm(blink_program);
	//sm.SetResource(pio1, 3);
	sm.ClaimResource();
	::printf("Running at pio%d sm%d offset:%d\n", sm.pio.get_index(), sm.sm, sm.offset);
	sm.config.set_set_pin(gpioLED);
	sm.pio.gpio_init(gpioLED);
	sm.set_pindir_out(gpioLED);
	sm.init();
	sm.set_enabled();
	sm.put((125000000 / (2 * freq)) - 3);
	for (;;) ::tight_loop_contents();
}
