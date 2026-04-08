//==============================================================================
// WS2812.cpp
//==============================================================================
#include "jxglib/Device/WS2812.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// Device::WS2812
// Minimum reset time: 280us
//------------------------------------------------------------------------------
void WS2812::Run(const GPIO& din)
{
	// low voltage must be between 580ns and 1000ns
	const int nClocksWhole	= 10;	// 1250ns
	const int nClocksLong	= 5;	// 625ns (must be between 580ns and 1000ns) + 625ns low
	const int nClocksShort	= 3;	// 375ns (must be between 220ns and 380ns) + 875ns low
	//--------------------------------------------------------------------------
	program_
	.pio_version(0)
	.program("ws2812")
	.side_set(1)
	.L("bitloop")
		.out("x", 1)			.side(0) [nClocksWhole - nClocksLong - 1]	// Side-set still takes place when instruction stalls
		.jmp("!x", "do_zero")	.side(1) [nClocksShort - 1]				 	// Branch on the bit we shifted out. Positive pulse
	.L("do_one")
		.jmp("bitloop")			.side(1) [nClocksLong - nClocksShort - 1]	// Continue driving high, for a long pulse
	.L("do_zero")
		.nop()					.side(0) [nClocksLong - nClocksShort - 1]	// Or drive low, for a short pulse
	.wrap()
	.end();
	//--------------------------------------------------------------------------
	sm_.set_program(program_)
		.reserve_sideset_pins(din, 1)
		.config_set_out_shift_left(true, 24)	// shift left, autopull enabled, pull threshold 24
		.config_set_fifo_join_tx()
		.config_set_clkdiv(static_cast<float>(::clock_get_hz(clk_sys)) / (Freq * nClocksWhole))
		.init()
		.set_enabled();
}

WS2812& WS2812::Put(uint8_t r, uint8_t g, uint8_t b)
{
	uint32_t data =
		((static_cast<uint32_t>(b) * brightness_ / 256) << 0) |
		((static_cast<uint32_t>(r) * brightness_ / 256) << 8) |
		((static_cast<uint32_t>(g) * brightness_ / 256) << 16);
	while (sm_.is_tx_fifo_full()) Tickable::TickSub();
	sm_.put(data << 8);
	return *this;
}

}
