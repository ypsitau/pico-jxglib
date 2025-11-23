//==============================================================================
// OV7670.cpp
//==============================================================================
#include "jxglib/Device/OV7670.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
OV7670::OV7670(i2c_inst_t* i2c, const PinAssign& pinAssign, uint32_t freq) : i2c_{i2c}, pinAssign_{pinAssign}, freq_{freq}
{
}

void OV7670::WriteReg(uint8_t reg, uint8_t value)
{
	uint8_t buf[2] = { reg, value };
	::i2c_write_blocking(i2c_, I2CAddr, buf, 2, false);
}

uint8_t OV7670::ReadReg(uint8_t reg)
{
	::i2c_write_blocking(i2c_, I2CAddr, &reg, 1, true);
	uint8_t value = 0;
	::i2c_read_blocking(i2c_, I2CAddr, &value, 1, false);
	return value;
}

void OV7670::Run()
{
	uint relAddrStart = 0;
#if 1
	program_
	.pio_version(0)
	.program("ov7670")
	.pub(&relAddrStart)
		.pull()
		.wait(1, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go high
		.wait(0, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go low
	.wrap_target()
		.wait(1, "gpio", pinAssign_.HREF)	// wait for HREF to go high
		.mov("x", "osr")
	.L("pixel")
		.wait(0, "gpio", pinAssign_.PLK)	// wait for PLK to go low
		.wait(1, "gpio", pinAssign_.PLK)	// wait for PLK to go high
		.in("pins", 8)
		.jmp("x--", "pixel")				// loop for the number of pixels in the line
		.wait(0, "gpio", pinAssign_.HREF)	// wait for HREF to go low
	.wrap()
	.end();
#else
	program_
	.pio_version(0)
	.program("ov7670")
	.pub(&relAddrStart)
		.wait(1, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go high
		.wait(0, "gpio", pinAssign_.VSYNC)	// wait for VSYNC to go low
	.wrap_target()
		.wait(1, "gpio", pinAssign_.HREF)	// wait for HREF to go high
	.L("pixel")
		.wait(0, "gpio", pinAssign_.PLK)	// wait for PLK to go low
		.wait(1, "gpio", pinAssign_.PLK)	// wait for PLK to go high
		.in("pins", 8)
		.jmp("pin", "pixel")				// loop while HREF is high
	.wrap()
	.end();
#endif
	//--------------------------------------------------------------------------
	sm_.set_program(program_)
		.reserve_in_pins(pinAssign_.DIN0, 8)
		.reserve_gpio_pin(pinAssign_.PLK, pinAssign_.HREF, pinAssign_.VSYNC)
		.config_set_in_shift_left(true, 32)	// shift left, autopush enabled, push threshold 32
		.config_set_jmp_pin(pinAssign_.HREF)
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
		.set_bswap(true)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	PWM(pinAssign_.XLK).set_function().set_freq(freq_).set_chan_duty(.5).set_enabled(true);
}

OV7670& OV7670::Capture(Image& image)
{
	if (!IsRunning()) return *this;
	sm_.set_enabled(false);
	sm_.clear_fifos().exec(programToReset_);
	pChannel_->set_config(channelConfig_)
		.set_read_addr(sm_.get_rxf())
		.set_write_addr(image.GetPointer())
		.set_trans_count_trig(image.GetBytesBuff() / sizeof(uint32_t));
	sm_.set_enabled();
	sm_.put(image.GetWidth() * 2 - 1);
	while (pChannel_->is_busy()) Tickable::Tick();
	return *this;
}

}
