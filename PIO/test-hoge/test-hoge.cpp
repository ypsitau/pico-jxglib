#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "blink.pio.h"

using namespace jxglib;

void blink_pin_forever(pio_t pio, uint sm, uint offset, uint pin, uint freq)
{
	pio_gpio_init(pio, pin);
	pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + blink_wrap_target, offset + blink_wrap);
	//pio_sm_config c = blink_program_get_default_config(offset);
	sm_config_set_set_pins(&c, pin, 1);
	pio_sm_init(pio, sm, offset, &c);
	pio_sm_set_enabled(pio, sm, true);

	printf("Blinking pin %d at %d Hz\n", pin, freq);

	// PIO counter program takes 3 more cycles in total than we pass as
	// input (wait for n + 1; mov; jmp)
	pio->txf[sm] = (125000000 / (2 * freq)) - 3;
}

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


	// PIO Blinking example
	pio_t pio = pio0;
	uint offset = pio_add_program(pio, blink_program_jxg);
	printf("Loaded program at %d\n", offset);
	
	blink_pin_forever(pio, 0, offset, 15, 3);
	// For more pio examples see https://github.com/raspberrypi/pico-examples/tree/master/pio

	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
