#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIOBox.h"
#include "blink.pio.h"

using namespace jxglib;

// pio_pins, pio_x, pio_y, pio_null, pio_pindirs, pio_exec_mov, pio_status, pio_pc, pio_isr, pio_osr, pio_exec_out

int main()
{
	::stdio_init_all();
	PIOProgram program;
	program
	.side_set(4)
	.wrap_target()
		.jmp	("")			[1]
		.set	(pio_x, 0)		.side(0b1)	[0]
		.mov	(pio_x, pio_y)
	.L("loop")
		.nop	()
		.jmp	("")
		.jmp	("!x",		"loop")
		.jmp	("x--",		"loop")
		.jmp	("!y",		"loop")
		.jmp	("y--",		"loop")
		.jmp	("x!=y",	"loop")
		.jmp	("pin",		"loop")
		.jmp	("!osre",	"loop")
	.wrap();
	PIOBox pioBox(PIOVAR_program(blink), PIOVAR_get_default_config(blink));
	pioBox.SetGPIO_out(GPIO3);
	pioBox.SetGPIO_in(GPIO4);
	pioBox.SetGPIO_set(GPIO5);
	pioBox.SetGPIO_sideset(GPIO6);
	pioBox.ClaimResource();
	pioBox.InitSM();
	pioBox.sm.put(0);
	pioBox.sm.get();
	pioBox.sm.is_rx_fifo_empty();
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
