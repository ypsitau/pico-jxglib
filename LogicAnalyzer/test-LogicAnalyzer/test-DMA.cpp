#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"

using namespace jxglib;

PIO::StateMachine sm;

TickableEntry(MonitorSM)
{
	if (!sm.is_rx_fifo_empty()) {
		uint32_t timeStamp = sm.get();
		uint32_t bits = sm.get();
		::printf("%08x: %08x\n", timeStamp, bits);
	}
}

int main()
{
	::stdio_init_all();
	const GPIO& gpio = GPIO2;
	uint nPins = 2;
	PIO::Program program;
	program
	.program("logic_analyzer")
	.L("loop").wrap_target()
		.mov("x", "~osr")
		.jmp("x--", "no_wrap_around")
		.mov("osr", "~x")
	.entry()
		.mov("isr", "null")
		.in("pins", nPins)			// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("do_report")	[1]
	.L("no_wrap_around")
		.mov("osr", "~x")			// increment osr (counter) by 1
		.mov("isr", "null")
		.in("pins", nPins)			// save current pins state in isr (no auto-push)
		.mov("x", "isr")
		.jmp("x!=y", "do_report")	// if pins state changed, report it
		.jmp("loop")		[2]
	.L("do_report")
		.in("osr", 32)				// auto-push osr (counter)
		.in("x", 32)				// auto-push x (current pins state)
		.mov("y", "x")				// save current pins state in y
	.wrap()
	.end();
	sm.config.set_in_shift_left(true, 32);	// shift left, autopush enabled, push threshold 32
	sm.set_program(program).set_listen_pins(gpio, -1).init().set_enabled();
	DMA::Channel& channel = *DMA::claim_unused_channel();
	DMA::ChannelConfig config;
	uint32_t dst[32];
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_32)
		.set_read_increment(false)
		.set_write_increment(true)
		.set_dreq(sm.get_dreq_rx()) // set DREQ of StateMachine's rx
		.set_chain_to(channel)		// disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	::printf("Transferred by DMA#%d: ", static_cast<uint>(channel));
	channel.set_config(config)
		.set_read_addr(sm.get_rxf())
		.set_write_addr(dst)
		.set_trans_count_trig(count_of(dst))
		.wait_for_finish_blocking();

	//-------------------------------------------------------------------------
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
