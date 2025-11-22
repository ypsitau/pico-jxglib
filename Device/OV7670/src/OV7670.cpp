//==============================================================================
// OV7670.cpp
//==============================================================================
#include "jxglib/Device/OV7670.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
OV7670::OV7670(const PinAssign& pinAssign, uint32_t freq) : pinAssign_{pinAssign}, freq_{freq}
{
}

void OV7670::Run()
{
	uint relAddrStart = 0;
	program_
	.pio_version(0)
	.program("ov7670")
	.pub(&relAddrStart)
		.wait(1, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go high
		.wait(0, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go low
	.wrap_target()
		.wait(1, "gpio", pinAssign_.HREF)	// wait for HREF to go high
	.L("pixel")
		.wait(1, "gpio", pinAssign_.PLK)	// wait for PLK to go high
		.in("pins", 8)
		.wait(0, "gpio", pinAssign_.PLK)	// wait for PLK to go low
		.jmp("pin", "pixel")				// if HREF is high, continue capturing pixels
	.wrap()
	.end();
	//--------------------------------------------------------------------------
	sm_.set_program(program_)
		.reserve_in_pins(pinAssign_.DIN0, 8)
		.reserve_gpio_pin(pinAssign_.PLK, pinAssign_.HREF, pinAssign_.VSYNC)
		.config_set_jmp_pin(pinAssign_.HREF)
		.config_set_in_shift_right(true, 32)	// shift right, autopush enabled, push threshold 32
		.config_set_fifo_join_rx()
		.init();
	programToReset_
	.pio_version(0)
	.program("ov7670_reset")
		.jmp(sm_.offset + relAddrStart)		// jump to the main program start
	.end();
	pChannel_ = DMA::claim_unused_channel();
	channelConfig_.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_32)
		.set_read_increment(false)
		.set_write_increment(true)
		.set_dreq(sm_.get_dreq_rx()) // set DREQ of StateMachine's rx
		.set_chain_to(*pChannel_)    // disable by setting chain_to to itself
		.set_ring_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	PWM(pinAssign_.XLK).set_function().set_freq(freq_).set_chan_duty(.5).set_enabled(true);
}

OV7670& OV7670::Capture(Image& image)
{
	if (!IsRunning()) return *this;
	sm_.set_enabled(false);
	sm_.exec(programToReset_);
	pChannel_->set_config(channelConfig_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(image.GetPointer())
		.set_trans_count_trig(image.GetBytesBuff() / sizeof(uint32_t));
	sm_.set_enabled();
	while (pChannel_->is_busy()) Tickable::Tick();
	return *this;
}

}
