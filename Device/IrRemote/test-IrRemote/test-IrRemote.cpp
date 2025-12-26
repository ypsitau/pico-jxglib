#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/PIO.h"

using namespace jxglib;

class IrRemote {
private:
	PIO::Program program_;
	PIO::Program programToReset_;
	PIO::StateMachine sm_;
	DMA::Channel* pChannel_;
	DMA::ChannelConfig channelConfig_;
	uint8_t buff_[64];
public:
	IrRemote();
public:
	void Run();
	void DoCapture();
};

IrRemote::IrRemote() : pChannel_{nullptr}
{
}

void IrRemote::Run()
{
	const uint8_t BURST_LOOP_COUNTER = 30;		// the detection threshold for a 'frame sync' burst
	const uint8_t BIT_SAMPLE_DELAY = 15;		// how long to wait after the end of the burst before sampling
	uint32_t relAddrStart = 0;
	program_
	.program("nec_receive")
	.wrap_target()
	.L("next_burst")
		.set("X", BURST_LOOP_COUNTER)
		.wait(0, "pin", 0)				// wait for the next burst to start
	.L("burst_loop")
		.jmp("pin", "data_bit")			// the burst ended before the counter expired
		.jmp("X--", "burst_loop")		// wait for the burst to end
										// the counter expired - this is a sync burst
		.mov("ISR", "NULL")				// reset the Input Shift Register
		.wait(1, "pin", 0)				// wait for the sync burst to finish
		.jmp("next_burst")				// wait for the first data bit
	.L("data_bit")
		.nop() [BIT_SAMPLE_DELAY - 1]	// wait for 1.5 burst periods before sampling the bit value
		.in("PINS", 1)					// if the next burst has started then detect a '0' (short gap)
										// otherwise detect a '1' (long gap)
										// after 32 bits the ISR will autopush to the receive FIFO
	.wrap()
	.end();
	sm_.set_program(program_)
		.reserve_in_pin(GPIO13)
		.config_set_fifo_join_rx()
		.config_set_jmp_pin(GPIO13)
		.config_set_clkdiv(::clock_get_hz(clk_sys) / (10.0 / 562.5e-6))
		.config_set_in_shift_right(true, 32)	// shift right, autopush enabled, push threshold 32
		.init()
		.set_enabled();
	GPIO13.pull_up();
	programToReset_
	.pio_version(0)
	.program("nec_receive_reset")
		.jmp(sm_.offset + relAddrStart)		// jump to the main program start
	.end();
	pChannel_ = DMA::claim_unused_channel();
	channelConfig_.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_32)
		.set_read_increment(false)
		.set_write_increment(true)
		.set_dreq(sm_.get_dreq_rx())		// set DREQ of StateMachine's rx
		.set_chain_to(*pChannel_)			// disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(true)					// byte swap: b0 b1 b2 b3 -> b3 b2 b1 b0
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
}

void IrRemote::DoCapture()
{
	if (!sm_.is_rx_fifo_empty()) {
		::printf("%08x\n", sm_.get());
	}
	//sm_.set_enabled(false);
	//pChannel_->set_config(channelConfig_)
	//	.set_read_addr(sm_.get_rxf())
	//	.set_write_addr(buff_)
	//	.set_trans_count_trig(1);
	//sm_.clear_fifos().exec(programToReset_).set_enabled();
	//while (pChannel_->is_busy()) Tickable::Tick();
	//::printf("Captured Data:%02x %02x %02x %02x\n", buff_[0], buff_[1], buff_[2], buff_[3]);
}

int main()
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	IrRemote irRemote;
	irRemote.Run();
	while (true) {
		irRemote.DoCapture();
		Tickable::Tick();
	}
}
