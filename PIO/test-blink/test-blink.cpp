#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	const GPIO& gpioLED1 = GPIO14;
	const GPIO& gpioLED2 = GPIO15;
	::stdio_init_all();
	PIO::Program program;
	program
	.program("auto_blink")
		.pull().block()				// osr <- txfifo
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
	PIO::StateMachine sm1;
	PIO::StateMachine sm2;
	sm1.ClaimResource(program);
	sm2.ClaimResource(sm1);
	sm1.config.set_set_pin(gpioLED1);
	sm2.config.set_set_pin(gpioLED2);
	sm1.pio.gpio_init(gpioLED1);
	sm2.pio.gpio_init(gpioLED2);
	sm1.set_pindir_out(gpioLED1);
	sm2.set_pindir_out(gpioLED2);
	sm1.init();
	sm2.init();
	sm1.set_enabled();
	sm2.set_enabled();
	sm1.put((::clock_get_hz(clk_sys) / (1 * 2)) - 3);
	sm2.put((::clock_get_hz(clk_sys) / (2 * 2)) - 3);
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
