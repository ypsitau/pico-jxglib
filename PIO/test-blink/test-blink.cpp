#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	PIO::Program program;
	program
	.program("hello")
	.L("loop")
		.set("pins", 1)
		//.pull()
		//.out("pins",	1)
		.jmp("loop")
	.end();
	PIO::StateMachine sm(program);
	if (!sm.ClaimResource()) {
		::printf("Failed to claim PIO resource\n");
		return 1;
	}
	sm.config.set_out_pins(GPIO15, 1);
	sm.pio.gpio_init(GPIO15);
	sm.set_consecutive_pindirs(GPIO15, 1, true);
	sm.init();
	sm.set_enabled(true);
	while (::getchar_timeout_us(0) == PICO_ERROR_TIMEOUT) {
		sm.put_blocking(1);
		::sleep_ms(500);
		sm.put_blocking(0);
		::sleep_ms(500);
	}
	sm.UnclaimResource();
	for (;;) ::tight_loop_contents();
}
