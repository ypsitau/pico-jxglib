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
	do {
		PIO::Program program;
		program
		.program("reverse_bit_order")
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
		num = 0x80000000; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x00000001; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0xaa00aa00; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	for (;;) Tickable::Tick();
}
