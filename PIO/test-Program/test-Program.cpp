#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "test-Program.pio.h"

using namespace jxglib;

void DumpInst(const pio_program_t& program);
void VerifyInst(const pio_program_t& program1, const pio_program_t& program2);

void CheckProgram(const PIO::Program& program, const pio_program_t& programExpected);

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
			.nop()
		.end();
		CheckProgram(program, JMP_Test_program);
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
		//	.wait	(1, "jmppin", 	0)
			.wait	(0, "gpio",		0)
			.wait	(0, "pin", 		0)
			.wait	(0, "irq", 		0)
		//	.wait	(0, "jmppin", 	0)
			.wait	(1, "gpio",		1)
			.wait	(1, "pin", 		1)
			.wait	(1, "irq", 		1)
		//	.wait	(1, "jmppin", 	1)
			.wait	(1, "gpio",		31)
			.wait	(1, "pin", 		31)
			.wait	(1, "irq", 		7)
		//	.wait	(1, "jmppin", 	3)
			.wait	(1, "irq", 		1).rel()
		//	.wait	(1, "irq", 		1).prev()
		//	.wait	(1, "irq", 		1).next()
		.end();
		CheckProgram(program, WAIT_Test_program);
	} while (0);
}

void test_IN()
{
	do {
		PIO::Program program;
		program
		.program("IN Test")
			.in		("pins",	32)
			.in		("x",		32)
			.in		("y",		32)
			.in		("null",	32)
			.in		("isr",		32)
			.in		("osr",		32)
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
		CheckProgram(program, IN_Test_program);
	} while (0);
}

void test_OUT()
{
	do {
		PIO::Program program;
		program
		.program("OUT Test")
			.out	("pins",	32)
			.out	("x",		32)
			.out	("y",		32)
			.out	("null",	32)
			.out	("pindirs",	32)
			.out	("pc",		32)
			.out	("isr",		32)
			.out	("exec",	32)
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
		CheckProgram(program, OUT_Test_program);
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
		CheckProgram(program, PUSH_Test_program);
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
		CheckProgram(program, PULL_Test_program);
	} while (0);
}

void test_MOV()
{
#if 0
	do {
		PIO::Program program;
		program
		.program("MOV rxfifo isr Test")
		.pio_version(1)
			.mov	("rxfifo[y]",	"isr")
			.mov	("rxfifo[0]",	"isr")
			.mov	("rxfifo[1]",	"isr")
			.mov	("rxfifo[7]",	"isr")
			.mov	("rxfifo[]", 0,	"isr")
			.mov	("rxfifo[]", 1,	"isr")
			.mov	("rxfifo[]", 7,	"isr")
		.end();
		CheckProgram(program, MOV_rxfifo_isr_Test_program);
	} while (0);
#endif
#if 0
	do {
		PIO::Program program;
		program
		.program("MOV osr rxfifo Test")
		.pio_version(1)
			.mov	("osr",		"rxfifo[y]")
			.mov	("osr",		"rxfifo[0]")
			.mov	("osr",		"rxfifo[1]")
			.mov	("osr",		"rxfifo[7]")
			.mov	("osr",		"rxfifo[]", 0)
			.mov	("osr",		"rxfifo[]", 1)
			.mov	("osr",		"rxfifo[]", 7)
		.end();
		CheckProgram(program, MOV_osr_rxfifo_Test_program);
	} while (0);
#endif
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
		CheckProgram(program, MOV_pins_Test_program);
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
		CheckProgram(program, MOV_x_Test_program);
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
		CheckProgram(program, MOV_y_Test_program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("MOV pindirs Test")
		.pio_version(1)
			.mov	("pindirs",		"pins")
			.mov	("pindirs",		"x")
			.mov	("pindirs",		"y")
			.mov	("pindirs",		"null")
			.mov	("pindirs",		"status")
			.mov	("pindirs",		"isr")
			.mov	("pindirs",		"osr")
		.end();
		CheckProgram(program, MOV_pindirs_Test_program);
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
		CheckProgram(program, MOV_exec_Test_program);
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
		CheckProgram(program, MOV_pc_Test_program);
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
		CheckProgram(program, MOV_isr_Test_program);
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
		CheckProgram(program, MOV_osr_Test_program);
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
		CheckProgram(program, MOV_operator_Test_program);
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
		//	.irq	("set", 		0).prev()
		//	.irq	("nowait",		0).prev()
		//	.irq	("wait",		0).prev()
		//	.irq	("clear",		0).prev()
		//	.irq	("set", 		0).next()
		//	.irq	("nowait",		0).next()
		//	.irq	("wait",		0).next()
		//	.irq	("clear",		0).next()
		.end();
		CheckProgram(program, IRQ_Test_program);
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
		CheckProgram(program, SET_Test_program);
	} while (0);
}

void test_SIDE_SET()
{
	do {
		PIO::Program program;
		program
		.program("SIDE_SET_1 Test")
		.side_set(1)
			.nop().side(0b0)
			.nop().side(0b1)
		.end();
		CheckProgram(program, SIDE_SET_1_Test_program);
		::printf("%s\n", program.GetName());
		DumpInst(program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("SIDE_SET_2 Test")
		.side_set(2)
			.nop().side(0b00)
			.nop().side(0b01)
			.nop().side(0b10)
			.nop().side(0b11)
		.end();
		CheckProgram(program, SIDE_SET_2_Test_program);
		::printf("%s\n", program.GetName());
		DumpInst(program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("SIDE_SET_5 Test")
		.side_set(5)
			.nop().side(0b00000)
			.nop().side(0b00001)
			.nop().side(0b00010)
			.nop().side(0b00100)
			.nop().side(0b01000)
			.nop().side(0b10000)
		.end();
		CheckProgram(program, SIDE_SET_5_Test_program);
		::printf("%s\n", program.GetName());
		DumpInst(program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("SIDE_SET_2 optional Test")
		.side_set(2).opt()
			.nop().side(0b00)
			.nop()
			.nop().side(0b01)
			.nop()
			.nop().side(0b10)
			.nop()
			.nop().side(0b11)
			.nop()
		.end();
		CheckProgram(program, SIDE_SET_2_optional_Test_program);
		::printf("%s\n", program.GetName());
		DumpInst(program);
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
	test_SIDE_SET();
	::printf("done\n");
	for (;;) ::tight_loop_contents();
}

void DumpInst(const pio_program_t& program)
{
	for (uint16_t addrRel = 0; addrRel < program.length; ++addrRel) {
		uint16_t inst = program.instructions[addrRel];
		::printf("%02x  %03b %05b %03b %05b\n", addrRel,
			(inst >> 13) & 0b111, (inst >> 8) & 0b11111, (inst >> 5) & 0b111, (inst >> 0) & 0b11111);
	}
}

void VerifyInst(const pio_program_t& program1, const pio_program_t& program2)
{
	if (program1.length != program2.length) {
		::printf("*** program length mismatch: %d vs %d\n", program1.length, program2.length);
		return;
	}
	for (uint16_t addrRel = 0; addrRel < program1.length; ++addrRel) {
		uint16_t inst1 = program1.instructions[addrRel];
		uint16_t inst2 = program2.instructions[addrRel];
		if (inst1 != inst2) {
			::printf("*** instruction mismatch at 0x%02x: [%03b %05b %03b %05b] vs [%03b %05b %03b %05b]\n", addrRel,
				(inst1 >> 13) & 0b111, (inst1 >> 8) & 0b11111, (inst1 >> 5) & 0b111, (inst1 >> 0) & 0b11111,
				(inst2 >> 13) & 0b111, (inst2 >> 8) & 0b11111, (inst2 >> 5) & 0b111, (inst2 >> 0) & 0b11111);
			return;
		}
	}
	::printf("programs match\n");
}

void CheckProgram(const PIO::Program& program, const pio_program_t& programExpected)
{
	::printf("%s\n", program.GetName());
	DumpInst(program);
	VerifyInst(program, programExpected);
}
