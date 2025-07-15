#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	const GPIO& gpioLED = GPIO15;
	::stdio_init_all();
	PIO::Program program;
	program
	.program("hello")
	.L("loop")
		.pull()
		.out("pins",	1)
		.jmp("loop")
	.end();
	PIO::StateMachine sm(program);
	sm.ClaimResource();
	sm.config.set_out_pin(gpioLED);
	sm.pio.gpio_init(gpioLED);
	sm.set_pindir_out(gpioLED);
	sm.init();
	sm.set_enabled();
	while (::getchar_timeout_us(0) == PICO_ERROR_TIMEOUT) {
		sm.put_blocking(1);
		::sleep_ms(500);
		sm.put_blocking(0);
		::sleep_ms(500);
	}
	sm.UnclaimResource();
	for (;;) ::tight_loop_contents();
}
