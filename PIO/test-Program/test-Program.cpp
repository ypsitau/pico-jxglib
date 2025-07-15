#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

using namespace jxglib;

void test_JMP()
{
	do {
		PIO::Program program;
		program
		.program("JMP Test")
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
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_WAIT()
{
	do {
		PIO::Program program;
		program
		.program("WAIT Test")
		.pio_version(1)
			.wait	(1, "gpio",		0)
			.wait	(1, "pin", 		0)
			.wait	(1, "irq", 		0)
			.wait	(1, "jmppin", 	0)
			.wait	(0, "gpio",		0)
			.wait	(0, "pin", 		0)
			.wait	(0, "irq", 		0)
			.wait	(0, "jmppin", 	0)
			.wait	(1, "gpio",		1)
			.wait	(1, "pin", 		1)
			.wait	(1, "irq", 		1)
			.wait	(1, "jmppin", 	1)
			.wait	(1, "gpio",		31)
			.wait	(1, "pin", 		31)
			.wait	(1, "irq", 		7)
			.wait	(1, "jmppin", 	3)
			.wait	(1, "irq", 		1).rel()
			.wait	(1, "irq", 		1).prev()
			.wait	(1, "irq", 		1).next()
		.end();
		::printf("%s\n", program.GetName());
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_IN()
{
	do {
		PIO::Program program;
		program
		.program("IN Test")
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
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_OUT()
{
	do {
		PIO::Program program;
		program
		.program("OUT Test")
			.out	("pins",	0)
			.out	("x",		0)
			.out	("y",		0)
			.out	("null",	0)
			.out	("pindirs",	0)
			.out	("pc",		0)
			.out	("isr",		0)
			.out	("exec",	0)
			.out	("pins",	1)
			.out	("x",		1)
			.out	("y",		1)
			.out	("null",	1)
			.out	("pindirs",	1)
			.out	("pc",		1)
			.out	("isr",		1)
			.out	("exec",	1)
			.out	("pins",	31)
			.out	("x",		31)
			.out	("y",		31)
			.out	("null",	31)
			.out	("pindirs",	31)
			.out	("pc",		31)
			.out	("isr",		31)
			.out	("exec",	31)
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_PUSH()
{
	do {
		PIO::Program program;
		program
		.program("PUSH Test")
			.push	()
			.push	().iffull()
			.push	().block()
			.push	().noblock()
			.push	().iffull().block()
			.push	().iffull().noblock()
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_PULL()
{
	do {
		PIO::Program program;
		program
		.program("PULL Test")
			.pull	()
			.pull	().ifempty()
			.pull	().block()
			.pull	().noblock()
			.pull	().ifempty().block()
			.pull	().ifempty().noblock()
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_MOV()
{
	do {
		PIO::Program program;
		program
		.program("MOV rxfifo[] Test")
		.pio_version(1)
			.mov	("rxfifo[y]",	"isr")
			.mov	("rxfifo[0]",	"isr")
			.mov	("rxfifo[1]",	"isr")
			.mov	("rxfifo[7]",	"isr")
			.mov	("rxfifo[]", 0,	"isr")
			.mov	("rxfifo[]", 1,	"isr")
			.mov	("rxfifo[]", 7,	"isr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV rxfifo[] Test")
		.pio_version(1)
			.mov	("osr",		"rxfifo[y]")
			.mov	("osr",		"rxfifo[0]")
			.mov	("osr",		"rxfifo[1]")
			.mov	("osr",		"rxfifo[7]")
			.mov	("osr",		"rxfifo[]", 0)
			.mov	("osr",		"rxfifo[]", 1)
			.mov	("osr",		"rxfifo[]", 7)
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV pins Test")
			.mov	("pins",		"pins")
			.mov	("pins",		"x")
			.mov	("pins",		"y")
			.mov	("pins",		"null")
			.mov	("pins",		"status")
			.mov	("pins",		"isr")
			.mov	("pins",		"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV x Test")
			.mov	("x",			"pins")
			.mov	("x",			"x")
			.mov	("x",			"y")
			.mov	("x",			"null")
			.mov	("x",			"status")
			.mov	("x",			"isr")
			.mov	("x",			"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV y Test")
			.mov	("y",			"pins")
			.mov	("y",			"x")
			.mov	("y",			"y")
			.mov	("y",			"null")
			.mov	("y",			"status")
			.mov	("y",			"isr")
			.mov	("y",			"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV pindirs Test")
			.mov	("pindirs",		"pins")
			.mov	("pindirs",		"x")
			.mov	("pindirs",		"y")
			.mov	("pindirs",		"null")
			.mov	("pindirs",		"status")
			.mov	("pindirs",		"isr")
			.mov	("pindirs",		"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV exec Test")
			.mov	("exec",		"pins")
			.mov	("exec",		"x")
			.mov	("exec",		"y")
			.mov	("exec",		"null")
			.mov	("exec",		"status")
			.mov	("exec",		"isr")
			.mov	("exec",		"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV pc Test")
			.mov	("pc",			"pins")
			.mov	("pc",			"x")
			.mov	("pc",			"y")
			.mov	("pc",			"null")
			.mov	("pc",			"status")
			.mov	("pc",			"isr")
			.mov	("pc",			"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV isr Test")
			.mov	("isr",			"pins")
			.mov	("isr",			"x")
			.mov	("isr",			"y")
			.mov	("isr",			"null")
			.mov	("isr",			"status")
			.mov	("isr",			"isr")
			.mov	("isr",			"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV osr Test")
			.mov	("osr",			"pins")
			.mov	("osr",			"x")
			.mov	("osr",			"y")
			.mov	("osr",			"null")
			.mov	("osr",			"status")
			.mov	("osr",			"isr")
			.mov	("osr",			"osr")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV operator Test")
			.mov	("pins",		"x")
			.mov	("pins",		"!x")
			.mov	("pins",		"~x")
			.mov	("pins",		"::x")
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_IRQ()
{
	do {
		PIO::Program program;
		program
		.program("IRQ Test")
			.irq	(0)
			.irq	("set", 		0)
			.irq	("nowait",		0)
			.irq	("wait",		0)
			.irq	("clear",		0)
			.irq	("set", 		1)
			.irq	("nowait",		1)
			.irq	("wait",		1)
			.irq	("clear",		1)
			.irq	("set", 		7)
			.irq	("nowait",		7)
			.irq	("wait",		7)
			.irq	("clear",		7)
			.irq	("set", 		0).rel()
			.irq	("nowait",		0).rel()
			.irq	("wait",		0).rel()
			.irq	("clear",		0).rel()
			.irq	("set", 		0).prev()
			.irq	("nowait",		0).prev()
			.irq	("wait",		0).prev()
			.irq	("clear",		0).prev()
			.irq	("set", 		0).next()
			.irq	("nowait",		0).next()
			.irq	("wait",		0).next()
			.irq	("clear",		0).next()
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
}

void test_SET()
{
	do {
		PIO::Program program;
		program
		.program("SET Test")
			.set	("pins",		0)
			.set	("x",			0)
			.set	("y",			0)
			.set	("pindirs",		0)
			.set	("pins",		1)
			.set	("x",			1)
			.set	("y",			1)
			.set	("pindirs",		1)
			.set	("pins",		31)
			.set	("x",			31)
			.set	("y",			31)
			.set	("pindirs",		31)
		.end();
		::printf("%s\n", program.GetName());
		program.Dump();
	} while (0);
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
	::printf("done\n");
	for (;;) ::tight_loop_contents();
}
