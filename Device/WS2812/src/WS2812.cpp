//==============================================================================
// WS2812.cpp
//==============================================================================
#include "jxglib/Device/WS2812.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// Device::WS2812
//------------------------------------------------------------------------------
void WS2812::Initialize(const GPIO& gpio, uint32_t freq)
{
	const int T1 = 3;
	const int T2 = 3;
	const int T3 = 4;
	//--------------------------------------------------------------------------
	program_
	.pio_version(0)
	.program("ws2812")
	.side_set(1)
	.L("bitloop")
		.out("x", 1)			.side(0) [T3 - 1] // Side-set still takes place when instruction stalls
		.jmp("!x", "do_zero")	.side(1) [T1 - 1] // Branch on the bit we shifted out. Positive pulse
	.L("do_one")
		.jmp("bitloop")			.side(1) [T2 - 1] // Continue driving high, for a long pulse
	.L("do_zero")
		.nop()					.side(0) [T2 - 1] // Or drive low, for a short pulse
	.wrap()
	.end();
	//--------------------------------------------------------------------------
	sm_.set_program(program_);
	sm_.reserve_sideset_pins(gpio, 1);
	sm_.config.set_out_shift_left(true, 24);	// shift left, autopull enabled, pull threshold 24
	sm_.config.set_fifo_join_tx();
	sm_.config.set_clkdiv(static_cast<float>(::clock_get_hz(clk_sys)) / (freq * (T1 + T2 + T3)));
	sm_.init();
	sm_.set_enabled();
}

const WS2812& WS2812::Put(uint8_t r, uint8_t g, uint8_t b) const
{
	uint32_t data =
		(static_cast<uint32_t>(b) << 0) |
		(static_cast<uint32_t>(r) << 8) |
		(static_cast<uint32_t>(g) << 16);
	while (sm_.is_tx_fifo_full()) Tickable::TickSub();
	sm_.put(data << 8);
	return *this;
}


}
