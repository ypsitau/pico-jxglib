#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "blink.pio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	PIO::Program blink_program_jxg;
	blink_program_jxg
	.program("blink")
		.pull().block()
		.out("y", 32)
	.wrap_target()
		.mov("x", "y")
		.set("pins", 1)		// Turn LED on
	.L("lp1")
		.jmp("x--", "lp1")	// Delay for (x + 1) cycles, x is a 32 bit number
		.mov("x", "y")
		.set("pins", 0)		// Turn LED off
	.L("lp2")
		.jmp("x--", "lp2")	// Delay for the same number of cycles again
	.wrap()				// Blink forever!
	.end();

	uint sm = 0;
	uint pin = 15;
	uint freq = 5;
	// PIO Blinking example
	PIO::Block pio = pio0;
	uint offset = pio_add_program(pio, blink_program_jxg);
	printf("Loaded program at %d\n", offset);

#if 1
	PIO::StateMachine sm0(blink_program_jxg);
	sm0.SetResource(pio, sm, offset);
	pio.gpio_init(pin);
	sm0.set_consecutive_pindirs(pin, 1, true);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + blink_wrap_target, offset + blink_wrap);
	sm_config_set_set_pins(&c, pin, 1);
	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);
#else
	pio_gpio_init(pio, pin);
	pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + blink_wrap_target, offset + blink_wrap);
	sm_config_set_set_pins(&c, pin, 1);
	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);
#endif
	pio->txf[sm] = (125000000 / (2 * freq)) - 3;
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
