#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	do {
		PIO::Program program;
		program
		.program("echo_back")
			.pull()
			.mov("isr", "osr")
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x89abcdef; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
#if 0
	do {
		PIO::Program program;
		program
		.program("echo_back_simplest")
			.out("isr", 32)
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x89abcdef; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
#endif
	do {
		PIO::Program program;
		program
		.program("bit_reverse")
			.pull()
			.mov("isr", "::osr")
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x00000000; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xffffffff; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x80808080; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x01010101; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xaa00aa00; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("bitwise_not")
			.pull()
			.mov("isr", "!osr")
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x00000000; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xffffffff; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x11111111; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xaaaaaaaa; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("six_bits_right_shift")
			.pull()
			.mov("isr", "osr")
			.in("null", 6)	// 6 bits shift right
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x00000000; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xffffffff; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x11111111; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xaaaaaaaa; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("six_bits_left_shift")
			.pull()
			.mov("isr", "osr")
			.in("null", 6)	// 6 bits shift left
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.config.set_in_shift_left();
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x00000000; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xffffffff; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x11111111; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xaaaaaaaa; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	for (;;) Tickable::Tick();
	do {
		PIO::Program program;
		program
		.program("six_bits_left_shift")
			.pull()
			.mov("isr", "osr")
			.in("null", 6)	// 6 bits shift left
			.push()
		.end();
		PIO::StateMachine sm(program);
		sm.config.set_in_shift_left();
		sm.ClaimResource().init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x00000000; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xffffffff; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x11111111; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xaaaaaaaa; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	for (;;) Tickable::Tick();
}
