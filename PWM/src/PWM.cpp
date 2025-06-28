//==============================================================================
// PWM.cpp
//==============================================================================
#include "jxglib/PWM.h"
#include "hardware/clocks.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PWM
//------------------------------------------------------------------------------
const PWM& PWM::set_freq(uint32_t freq) const
{
	float clkdiv;
	uint16_t wrap;
	CalcClkdivAndWrap(freq, &clkdiv, &wrap);
	set_clkdiv(clkdiv);
	set_wrap(wrap);
	return *this;
}

const PWM& PWM::set_duty(float duty) const
{
	duty = (duty < 0.0f)? 0.0f : (duty > 1.0f)? 1.0f : duty;
	set_chan_level(static_cast<uint16_t>(duty * get_wrap()));
	return *this;
}

float PWM::get_duty() const
{
	// Get current wrap and level values
	uint16_t wrap = get_wrap();
	if (wrap == 0) return 0.0f;
	
	// Get current channel level
	uint slice_num = GetSliceNum();
	uint channel = GetChannel();
	
	uint16_t level;
	if (channel == PWM_CHAN_A) {
		level = (pwm_hw->slice[slice_num].cc & PWM_CH0_CC_A_BITS) >> PWM_CH0_CC_A_LSB;
	} else {
		level = (pwm_hw->slice[slice_num].cc & PWM_CH0_CC_B_BITS) >> PWM_CH0_CC_B_LSB;
	}
	
	// Calculate duty ratio
	return static_cast<float>(level) / static_cast<float>(wrap);
}

void PWM::CalcClkdivAndWrap(uint32_t freq, float* pClkdiv, uint16_t* pWrap)
{
	// Get system clock frequency from Pico SDK
	const uint32_t clk_sys_freq = clock_get_hz(clk_sys);
	
	*pClkdiv = 1.0f;
	*pWrap = 65535;
	
	// Do nothing if target frequency is 0
	if (freq == 0) return;
	
	// Find optimal combination of wrap and clkdiv
	// PWM frequency: freq = clk_sys / (clkdiv * (wrap + 1))
	// Target: clkdiv * (wrap + 1) = clk_sys / freq
	
	uint32_t target_divisor = clk_sys_freq / freq;
	float best_clkdiv = 1.0f;
	uint16_t best_wrap = 65535;
	uint32_t best_error = UINT32_MAX;
	
	// Search clkdiv in range 1.0 to 256.0
	for (uint32_t clkdiv_int = 1; clkdiv_int <= 256; clkdiv_int++) {
		// Calculate optimal wrap for each integer clkdiv
		uint32_t wrap_plus_1 = target_divisor / clkdiv_int;
		
		if (wrap_plus_1 == 0 || wrap_plus_1 > 65536) continue;
		
		uint16_t wrap = static_cast<uint16_t>(wrap_plus_1 - 1);
		uint32_t actual_divisor = clkdiv_int * (wrap + 1);
		uint32_t actual_freq = clk_sys_freq / actual_divisor;
		
		// Calculate error (absolute difference)
		uint32_t error = (actual_freq > freq) ? (actual_freq - freq) : (freq - actual_freq);
		
		if (error < best_error) {
			best_error = error;
			best_clkdiv = static_cast<float>(clkdiv_int);
			best_wrap = wrap;
		}
	}
	
	// Try fractional parts for finer adjustment
	for (uint32_t clkdiv_int = 1; clkdiv_int <= 255; clkdiv_int++) {
		for (uint32_t frac = 1; frac <= 15; frac++) {
			float clkdiv_frac = static_cast<float>(clkdiv_int) + static_cast<float>(frac) / 16.0f;
			uint32_t wrap_plus_1 = static_cast<uint32_t>(target_divisor / clkdiv_frac);
			
			if (wrap_plus_1 == 0 || wrap_plus_1 > 65536) continue;
			
			uint16_t wrap = static_cast<uint16_t>(wrap_plus_1 - 1);
			uint32_t actual_freq = static_cast<uint32_t>(clk_sys_freq / (clkdiv_frac * (wrap + 1)));
			
			uint32_t error = (actual_freq > freq) ? (actual_freq - freq) : (freq - actual_freq);
			
			if (error < best_error) {
				best_error = error;
				best_clkdiv = clkdiv_frac;
				best_wrap = wrap;
			}
		}
	}
	
	// Set optimal values
	*pClkdiv = best_clkdiv;
	*pWrap = best_wrap;
}

uint32_t PWM::CalcFreq(float clkdiv, uint16_t wrap)
{
	// Get system clock frequency from Pico SDK
	const uint32_t clk_sys_freq = clock_get_hz(clk_sys);
	if (clkdiv < 1.0f || clkdiv >= 256.0f || wrap == 0) return 0;
	return clk_sys_freq / (static_cast<uint32_t>(clkdiv * (wrap + 1)));
}

uint32_t PWM::CalcFreq(uint8_t div_int, uint8_t div_frac, uint16_t wrap)
{
	if (div_int < 1 || div_int > 256 || div_frac >= 16 || wrap == 0) return 0;
	float clkdiv = static_cast<float>(div_int) + static_cast<float>(div_frac) / 16.0f;
	return CalcFreq(clkdiv, wrap);
}

float PWM::get_clkdiv(uint slice_num)
{
	check_slice_num_param(slice_num);
	uint32_t div_reg = pwm_hw->slice[slice_num].div;
	uint8_t div_int = (div_reg & PWM_CH0_DIV_INT_BITS) >> PWM_CH0_DIV_INT_LSB;
	uint8_t div_frac = (div_reg & PWM_CH0_DIV_FRAC_BITS) >> PWM_CH0_DIV_FRAC_LSB;
	return static_cast<float>(div_int) + static_cast<float>(div_frac) / 16.0f;
}

uint16_t PWM::get_wrap(uint slice_num)
{
	check_slice_num_param(slice_num);
	return static_cast<uint16_t>(pwm_hw->slice[slice_num].top);
}

void PWM::set_chan_output_polarity(uint slice_num, uint chan, bool inv)
{
	check_slice_num_param(slice_num);
	if (chan == PWM_CHAN_A) {
		hw_write_masked(&pwm_hw->slice[slice_num].csr, bool_to_bit(inv) << PWM_CH0_CSR_A_INV_LSB, PWM_CH0_CSR_A_INV_BITS);
	} else {
		hw_write_masked(&pwm_hw->slice[slice_num].csr, bool_to_bit(inv) << PWM_CH0_CSR_B_INV_LSB, PWM_CH0_CSR_B_INV_BITS);
	}
}

bool PWM::is_enabled(uint slice_num)
{
	check_slice_num_param(slice_num);
	return (pwm_hw->slice[slice_num].csr & PWM_CH0_CSR_EN_BITS) != 0;
}

uint16_t PWM::get_chan_level(uint slice_num, uint channel)
{
	return (channel == PWM_CHAN_A)?
		((pwm_hw->slice[slice_num].cc & PWM_CH0_CC_A_BITS) >> PWM_CH0_CC_A_LSB) :
		((pwm_hw->slice[slice_num].cc & PWM_CH0_CC_B_BITS) >> PWM_CH0_CC_B_LSB);
}

}
