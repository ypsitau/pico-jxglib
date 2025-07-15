#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	const GPIO& gpioLED = GPIO15;
	::stdio_init_all();
	PIO::Program program;
#if 1
	program
	.program("manual-blink")
		.pull().block()				// osr <- txfifo
		.out("pins",	1)			// pins <- osr[0], osr >>= 1
	.end();
#else
	program
	.program("auto-blink")
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
#endif
	PIO::StateMachine sm(program);
	sm.ClaimResource();
	sm.config.set_set_pin(gpioLED);
	sm.config.set_out_pin(gpioLED);
	sm.pio.gpio_init(gpioLED);
	sm.set_pindir_out(gpioLED);
	sm.init();
	sm.set_enabled();
#if 1
	while (::getchar_timeout_us(0) == PICO_ERROR_TIMEOUT) {
		sm.put_blocking(1);
		::sleep_ms(500);
		sm.put_blocking(0);
		::sleep_ms(500);
	}
	sm.UnclaimResource();
#else
	sm.put((125000000 / (2 * 2)) - 3);
#endif
	for (;;) ::tight_loop_contents();
}
