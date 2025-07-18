#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "test-Examples.pio.h"

using namespace jxglib;

void DumpInst(const pio_program_t& program);
void VerifyInst(const pio_program_t& program1, const pio_program_t& program2);

void CheckProgram(const PIO::Program& program, const pio_program_t& programExpected);

int main()
{
	::stdio_init_all();
	do {
		PIO::Program program;
		program
		.program("spi_cpha0")
		.side_set(1)
			.out("pins", 1)		.side(0)	[1] // Stall here on empty (sideset proceeds even if
			.in("pins", 1)		.side(1)	[1] // instruction stalls, so we stall with SCK low)
		.end();
		CheckProgram(program, spi_cpha0_program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("spi_cpha1")
		.side_set(1)
			.out("x", 1)		.side(0)		// Stall here on empty (keep SCK deasserted)
			.mov("pins", "x")	.side(1)	[1]	// Output data, assert SCK (mov pins uses OUT mapping)
			.in("pins", 1)		.side(0)		// Input data, deassert SCK
		.end();
		CheckProgram(program, spi_cpha1_program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("uart_rx")
		.L("start")
			.wait(0, "pin", 0)					// Stall until start bit is asserted
			.set("x", 7)					[10]// Preload bit counter, then delay until halfway through
		.L("bitloop")
			.in("pins", 1)						// Shift data bit into ISR
			.jmp("x--", "bitloop")			[6]	// Loop 8 times, each loop iteration is 8 cycles
			.jmp("pin", "good_stop")			// Check stop bit (should be high)
			.irq(4).rel()						// Either a framing error or a break. Set a sticky flag,
			.wait(1, "pin", 0)					// and wait for line to return to idle state.
			.jmp("start")						// Don't push data if we didn't see good framing.
		.L("good_stop")							// No delay before returning to start; a little slack is
			.push()								// important in case the TX clock is slightly too fast
		.end();
		CheckProgram(program, uart_rx_program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("uart_tx")
		.side_set(1).opt()
			.pull()			.side(1)		[7]	// Assert stop bit, or stall with line in idle state
			.set("x", 7)	.side(0)		[7]	// Preload bit counter, assert start bit for 8 clocks
		.L("bitloop")							// This loop will run 8 times (8n1 UART)
			.out("pins", 1)						// Shift 1 bit from OSR to the first OUT pin
			.jmp("x--", "bitloop")			[6]	// Each loop iteration is 8 cycles.
		.end();
		CheckProgram(program, uart_tx_program);
	} while (0);
	do {
		const uint T1 = 3;
		const uint T2 = 3;
		const uint T3 = 4;
		PIO::Program program;
		program
		.program("ws2812")
		.side_set(1)
		.wrap_target()
		.L("bitloop")
			.out("x", 1)			.side(0)	[T3 - 1]	// Side-set still takes place when instruction stalls
			.jmp("!x", "do_zero")	.side(1)	[T1 - 1]	// Branch on the bit we shifted out. Positive pulse
		.L("do_one")
			.jmp("bitloop")			.side(1)	[T2 - 1]	// Continue driving high, for a long pulse
		.L("do_zero")
			.nop()					.side(0)	[T2 - 1]	// Or drive low, for a short pulse
		.wrap()
		.end();
		CheckProgram(program, ws2812_program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("manchester_tx")
		.side_set(1).opt()
		.wrap_target()
		.L("do_1")
			.nop()				.side(0)		[5]	// Low for 6 cycles (5 delay, +1 for nop)
			.jmp("get_bit")		.side(1)		[3]	// High for 4 cycles. 'get_bit' takes another 2 cycles
		.L("do_0")
			.nop()				.side(1)		[5]	// Output high for 6 cycles
			.nop()				.side(0)		[3]	// Output low for 4 cycles
		.L("start").entry()
		.L("get_bit")
			.out("x", 1)							// Always shift out one bit from OSR to X, so we can
			.jmp("!x", "do_0")						// branch on it. Autopull refills the OSR when empty.
		.wrap()
		.end();
		CheckProgram(program, manchester_tx_program);
	} while (0);
	do {
		PIO::Program program;
		program
		.program("pwm")
		.side_set(1).opt()
			.pull().noblock()	.side(0)			// Pull from FIFO to OSR if available, else copy X to OSR.
			.mov("x", "osr")						// Copy most-recently-pulled value back to scratch X
			.mov("y", "isr")						// ISR contains PWM period. Y used as counter.
		.L("countloop")
			.jmp("x!=y", "noset")					// Set pin high if X == Y, keep the two paths length matched
			.jmp("skip")		.side(1)
		.L("noset")
			.nop()									// Single dummy cycle to keep the two paths the same length
		.L("skip")
			.jmp("y--", "countloop")				// Loop until Y hits 0, then pull a fresh PWM value from FIFO
		.end();
		CheckProgram(program, pwm_program);
	} while (0);
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
