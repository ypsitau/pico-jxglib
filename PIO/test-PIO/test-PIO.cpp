#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "blink.pio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	PIO::Program program;
	program
	.side_set(4).opt().pindirs()
	.wrap_target()
		.jmp	("a")							[1]
		.set	(pio_x,		0)		.side(0b1)	[0]
		.mov	(pio_x,		pio_y)
	.L("loop")
		.nop	()
		.mov	("x",		"y")
		.mov	("x",		"!y")
		.mov	("x",		"::y")
		.jmp	("!x",		"loop")
		.jmp	("x--",		"loop")
		.jmp	("!y",		"loop")
		.jmp	("y--",		"loop")
		.jmp	("x!=y",	"loop")
		.jmp	("pin",		"loop")
		.jmp	("!osre",	"loop")
		.wait	("gpio",	0)
		.wait	("!gpio",	0)
		.wait	("pin",		0)
		.wait	("!pin",	0)
		.wait	("irq",		0)
		.wait	("!irq",	0)
		.wait	("irq",		0).rel()
		.wait	("!irq",	0).rel()
	.wrap();
	program.Resolve().Dump();
#if 0
	PIO::Controller pioCtrl(PIOVAR_program(blink), PIOVAR_get_default_config(blink));
	pioCtrl.SetGPIO_out(GPIO3);
	pioCtrl.SetGPIO_in(GPIO4);
	pioCtrl.SetGPIO_set(GPIO5);
	pioCtrl.SetGPIO_sideset(GPIO6);
	pioCtrl.ClaimResource();
	pioCtrl.InitSM();
	pioCtrl.sm.put(0);
	pioCtrl.sm.get();
	pioCtrl.sm.is_rx_fifo_empty();
#endif
	::printf("done\n");
	for (;;) ::tight_loop_contents();
}
