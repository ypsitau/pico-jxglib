#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "blink.pio.h"

using namespace jxglib;

void test_JMP()
{
	PIO::Program program;
	program
	.side_set(4).opt().pindirs()
	.wrap_target()
		.jmp	("label")
		.jmp	("!x",		"label")
		.jmp	("x--",		"label")
		.jmp	("!y",		"label")
		.jmp	("y--",		"label")
		.jmp	("x!=y",	"label")
		.jmp	("pin",		"label")
		.jmp	("!osre",	"label")
	.wrap();
	program.Resolve().Dump();
}

void test_WAIT()
{
}

void test_IN()
{
}

void test_OUT()
{
}

void test_PUSH()
{
}

void test_PULL()
{
}

void test_MOV()
{
}

void test_IRQ()
{
}

void test_SET()
{
}

int main()
{
	::stdio_init_all();
	test_JMP();
	test_WAIT();
	test_IN();
	test_OUT();
	test_PUSH();
	test_PULL();
	test_MOV();
	test_IRQ();
	test_SET();
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
