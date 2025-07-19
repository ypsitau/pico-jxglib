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
		.program("pull_mov_push")
			.pull()				// osr <- txfifo
			.mov("isr", "osr")	// isr <- osr
			.push()				// rxfifo <- isr
		.end();
		PIO::StateMachine sm;
		sm.ClaimResource(program).init();
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
		.program("autopull_autopush_32bits")
			.out("x", 32)	// auto-pull (osr <- txfifo), x[31:0] <- osr[31:0], osr[31:0] = 0
			.in("x", 32)	// isr[31:0] <- x[31:0], auto-push (rxfifo <- isr)
		.end();
		PIO::StateMachine sm;
		sm.config.set_out_shift_right(true, 32);	// see 3.5.4. Autopush and Autopull
		sm.config.set_in_shift_right(true, 32);		// see 3.5.4. Autopush and Autopull
		sm.ClaimResource(program).init();
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
		.program("autopull_right_8bits")
			.out("isr", 8)	// auto-pull (osr <- txfifo) when empty, x <- osr[7:0], osr[23:0] = osr[31:8], osr[31:24] = 0
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_out_shift_right(true, 32);	// see 3.5.4. Autopush and Autopull
		sm.ClaimResource(program).init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; sm.put(num); printf("%08x -> %02x, %02x, %02x, %02x\n", num, sm.get(), sm.get(), sm.get(), sm.get());
		num = 0x89abcdef; sm.put(num); printf("%08x -> %02x, %02x, %02x, %02x\n", num, sm.get(), sm.get(), sm.get(), sm.get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("autopull_right_4bits")
			.out("isr", 4)	// auto-pull (osr <- txfifo) when empty, x <- osr[3:0], osr[27:0] = osr[31:28], osr[31:28] = 0
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_out_shift_right(true, 32);	// see 3.5.4. Autopush and Autopull
		sm.ClaimResource(program).init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; sm.put(num); printf("%08x -> %x, %x, %x, %x, %x, %x, %x, %x\n",
				num, sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get());
		num = 0x89abcdef; sm.put(num); printf("%08x -> %x, %x, %x, %x, %x, %x, %x, %x\n",
				num, sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("autopull_left_8bits")
			.out("isr", 8)	// auto-pull (osr <- txfifo) when empty, x <- osr[31:24], osr[31:8] = osr[23:0], osr[7:0] = 0
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_out_shift_left(true, 32);	// see 3.5.4. Autopush and Autopull
		sm.ClaimResource(program).init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; sm.put(num); printf("%08x -> %02x, %02x, %02x, %02x\n", num, sm.get(), sm.get(), sm.get(), sm.get());
		num = 0x89abcdef; sm.put(num); printf("%08x -> %02x, %02x, %02x, %02x\n", num, sm.get(), sm.get(), sm.get(), sm.get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("autopull_left_4bits")
			.out("isr", 4)	// auto-pull (osr <- txfifo) when empty, x <- osr[31:28], osr[31:4] = osr[27:0], osr[3:0] = 0
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_out_shift_left(true, 32);	// see 3.5.4. Autopush and Autopull
		sm.ClaimResource(program).init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; sm.put(num); printf("%08x -> %x, %x, %x, %x, %x, %x, %x, %x\n",
				num, sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get());
		num = 0x89abcdef; sm.put(num); printf("%08x -> %x, %x, %x, %x, %x, %x, %x, %x\n",
				num, sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get(), sm.get());
		sm.UnclaimResource();
	} while (0);
	do {
		PIO::Program program;
		program
		.program("bit_reverse")
			.pull()
			.mov("isr", "::osr")
			.push()
		.end();
		PIO::StateMachine sm;
		sm.ClaimResource(program).init();
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
		PIO::StateMachine sm;
		sm.ClaimResource(program).init();
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
		.program("four_bits_right_shift")
			.pull()
			.mov("isr", "osr")
			.in("null", 4)			// isr[27:0] <- isr[31:4], isr[31:28] <- null[3:0]
			.push()
		.end();
		PIO::StateMachine sm;
		sm.ClaimResource(program).init();
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
		.program("four_bits_right_shift_0xb")
			.pull()
			.mov("isr", "osr")
			.set("x", 0xb)
			.in("x", 4)				// isr[27:0] <- isr[31:4], isr[31:28] <- x[3:0]
			.push()
		.end();
		PIO::StateMachine sm;
		sm.ClaimResource(program).init();
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
		.program("four_bits_left_shift")
			.pull()
			.mov("isr", "osr")
			.in("null", 4)			// isr[31:4] <- isr[27:0], isr[3:0] <- 0
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_in_shift_left();
		sm.ClaimResource(program).init();
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
		.program("four_bits_left_shift")
			.pull()
			.mov("isr", "osr")
			.set("x", 0xb)
			.in("x", 4)				// isr[31:4] <- isr[27:0], isr[3:0] <- x[3:0]
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_in_shift_left();
		sm.ClaimResource(program).init();
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
		.program("four_bits_right_rotate")
			.pull()
			.mov("isr", "osr")
			.in("isr", 4)			// isr[27:0] <- isr[31:4], isr[31:28] <- isr[3:0]
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_in_shift_right();
		sm.ClaimResource(program).init();
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
		.program("four_bits_left_rotate")
			.pull()
			.mov("isr", "osr")
			.in("isr", 4)			// isr[31:4] <- isr[27:0], isr[3:0] <- isr[3:0]
			.push()
		.end();
		PIO::StateMachine sm;
		sm.config.set_in_shift_left();
		sm.ClaimResource(program).init();
		sm.set_enabled();
		uint32_t num;
		::printf("program: %s\n", program.GetName());
		num = 0x01234567; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		num = 0x89abcdef; ::printf("%08x -> %08x\n", num, sm.put(num).get());
		sm.UnclaimResource();
	} while (0);
	::printf("done\n");
	for (;;) Tickable::Tick();
}
