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
			.wait	(1, "gpio",	0)
			.wait	(1, "pin", 	0)
			.wait	(1, "irq", 	0)
			.wait	(0, "gpio",	0)
			.wait	(0, "pin", 	0)
			.wait	(0, "irq", 	0)
			.wait	(1, "gpio",	1)
			.wait	(1, "pin", 	1)
			.wait	(1, "irq", 	1)
			.wait	(1, "gpio",	3)
			.wait	(1, "pin", 	3)
			.wait	(1, "irq", 	3)
			.wait	(1, "gpio",	31)
			.wait	(1, "pin", 	31)
			.wait	(1, "irq", 	1)		.rel()
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
	do {
		PIO::Program program;
		program
			.mov		("rxfifo[y]",	"isr")
			.mov		("rxfifo[0]",	"isr")
			.mov		("rxfifo[1]",	"isr")
			.mov		("rxfifo[7]",	"isr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("osr",		"rxfifo[y]")
			.mov		("osr",		"rxfifo[0]")
			.mov		("osr",		"rxfifo[1]")
			.mov		("osr",		"rxfifo[7]")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("pins",		"pins")
			.mov		("pins",		"x")
			.mov		("pins",		"y")
			.mov		("pins",		"null")
			.mov		("pins",		"status")
			.mov		("pins",		"isr")
			.mov		("pins",		"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("x",			"pins")
			.mov		("x",			"x")
			.mov		("x",			"y")
			.mov		("x",			"null")
			.mov		("x",			"status")
			.mov		("x",			"isr")
			.mov		("x",			"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("y",			"pins")
			.mov		("y",			"x")
			.mov		("y",			"y")
			.mov		("y",			"null")
			.mov		("y",			"status")
			.mov		("y",			"isr")
			.mov		("y",			"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("pindirs",		"pins")
			.mov		("pindirs",		"x")
			.mov		("pindirs",		"y")
			.mov		("pindirs",		"null")
			.mov		("pindirs",		"status")
			.mov		("pindirs",		"isr")
			.mov		("pindirs",		"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("exec",		"pins")
			.mov		("exec",		"x")
			.mov		("exec",		"y")
			.mov		("exec",		"null")
			.mov		("exec",		"status")
			.mov		("exec",		"isr")
			.mov		("exec",		"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("pc",			"pins")
			.mov		("pc",			"x")
			.mov		("pc",			"y")
			.mov		("pc",			"null")
			.mov		("pc",			"status")
			.mov		("pc",			"isr")
			.mov		("pc",			"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("isr",			"pins")
			.mov		("isr",			"x")
			.mov		("isr",			"y")
			.mov		("isr",			"null")
			.mov		("isr",			"status")
			.mov		("isr",			"isr")
			.mov		("isr",			"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("osr",			"pins")
			.mov		("osr",			"x")
			.mov		("osr",			"y")
			.mov		("osr",			"null")
			.mov		("osr",			"status")
			.mov		("osr",			"isr")
			.mov		("osr",			"osr")
		.end();
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
			.mov		("pins",		"x")
			.mov		("pins",		"!x")
			.mov		("pins",		"~x")
			.mov		("pins",		"::x")
		.end();
		program.Dump();
	} while (0);
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
#if 1
	PIO::Controller pioCtrl(PIOVAR_program(blink), PIOVAR_get_default_config(blink));
	pioCtrl.Set_out(GPIO3);
	pioCtrl.Set_in(GPIO4);
	pioCtrl.Set_set(GPIO5);
	pioCtrl.Set_sideset(GPIO6);
	pioCtrl.ClaimResource();
	pioCtrl.InitSM();
	pioCtrl.sm.put(0);
	pioCtrl.sm.get();
	pioCtrl.sm.is_rx_fifo_empty();
#endif
	::printf("done\n");
	for (;;) ::tight_loop_contents();
}
