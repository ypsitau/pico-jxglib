#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "blink.pio.h"

using namespace jxglib;

void test_JMP()
{
	::printf("JMP:\n");
	do {
		PIO::Program program;
		program
			.jmp	("label")
			.jmp	("!x",		"label")
			.jmp	("x--",		"label")
			.jmp	("!y",		"label")
			.jmp	("y--",		"label")
			.jmp	("x!=y",	"label")
			.jmp	("pin",		"label")
			.jmp	("!osre",	"label")
		.L("label")
		.end();
		program.Dump();
	} while (0);
}

void test_WAIT()
{
	::printf("WAIT:\n");
	do {
		PIO::Program program;
		program
			.wait	("gpio",	0)
			.wait	("pin", 	0)
			.wait	("irq", 	0)
			.wait	("!gpio",	0)
			.wait	("!pin", 	0)
			.wait	("!irq", 	0)
			.wait	("gpio",	1)
			.wait	("pin", 	1)
			.wait	("irq", 	1)
			.wait	("gpio",	3)
			.wait	("pin", 	3)
			.wait	("irq", 	3)
			.wait	("gpio",	31)
			.wait	("pin", 	31)
			.wait	("irq", 	1)		.rel()
		.end();
		program.Dump();
	} while (0);
}

void test_IN()
{
	::printf("IN:\n");
	do {
		PIO::Program program;
		program
			.in		("pins",	0)
			.in		("x",		0)
			.in		("y",		0)
			.in		("null",	0)
			.in		("isr",		0)
			.in		("osr",		0)
			.in		("pins",	1)
			.in		("x",		1)
			.in		("y",		1)
			.in		("null",	1)
			.in		("isr",		1)
			.in		("osr",		1)
			.in		("pins",	31)
			.in		("x",		31)
			.in		("y",		31)
			.in		("null",	31)
			.in		("isr",		31)
			.in		("osr",		31)
		.end();
		program.Dump();
	} while (0);
}

void test_OUT()
{
	::printf("OUT:\n");
	do {
		PIO::Program program;
		program
			.out		("pins",	0)
			.out		("x",		0)
			.out		("y",		0)
			.out		("null",	0)
			.out		("pindirs",	0)
			.out		("pc",		0)
			.out		("isr",		0)
			.out		("exec",	0)
			.out		("pins",	1)
			.out		("x",		1)
			.out		("y",		1)
			.out		("null",	1)
			.out		("pindirs",	1)
			.out		("pc",		1)
			.out		("isr",		1)
			.out		("exec",	1)
			.out		("pins",	31)
			.out		("x",		31)
			.out		("y",		31)
			.out		("null",	31)
			.out		("pindirs",	31)
			.out		("pc",		31)
			.out		("isr",		31)
			.out		("exec",	31)
		.end();
		program.Dump();
	} while (0);
}

void test_PUSH()
{
	::printf("PUSH:\n");
	do {
		PIO::Program program;
		program
			.push		()
			.push		().iffull()
			.push		().block()
			.push		().noblock()
			.push		().iffull().block()
			.push		().iffull().noblock()
		.end();
		program.Dump();
	} while (0);
}

void test_PULL()
{
	::printf("PULL:\n");
	do {
		PIO::Program program;
		program
			.pull		()
			.pull		().ifempty()
			.pull		().block()
			.pull		().noblock()
			.pull		().ifempty().block()
			.pull		().ifempty().noblock()
		.end();
		program.Dump();
	} while (0);
}

void test_MOV()
{
	::printf("MOV:\n");
}

void test_IRQ()
{
	::printf("IRQ:\n");
}

void test_SET()
{
	::printf("SET:\n");
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
