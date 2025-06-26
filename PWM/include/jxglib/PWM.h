//==============================================================================
// jxglib/PWM.h
//==============================================================================
#ifndef PICO_JXGLIB_PWM_H
#define PICO_JXGLIB_PWM_H
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PWM
//------------------------------------------------------------------------------
class PWM {
public:
	class Config {
	private:
		pwm_config config_;
	public:
		Config() : config_{::pwm_get_default_config()} {}
		Config(const pwm_config& config) : config_{config} {}
		Config(const Config& config) : config_{config.config_} {}
	public:
		pwm_config& GetEntity() { return config_; }
		const pwm_config& GetEntity() const { return config_; }
		pwm_config* GetEntityPtr() { return &config_; }
		const pwm_config* GetEntityPtr() const { return &config_; }
	public:
		Config& set_phase_correct(bool phase_correct) { ::pwm_config_set_phase_correct(&config_, phase_correct); return *this; }
		Config& set_clkdiv(float div) { ::pwm_config_set_clkdiv(&config_, div); return *this; }
		Config& set_clkdiv_int_frac(uint8_t integer, uint8_t fract) { ::pwm_config_set_clkdiv_int_frac(&config_, integer, fract); return *this; }
		Config& set_clkdiv_int(uint div) { ::pwm_config_set_clkdiv_int(&config_, div); return *this; }
		Config& set_clkdiv_mode(enum pwm_clkdiv_mode mode) { ::pwm_config_set_clkdiv_mode(&config_, mode); return *this; }
		Config& set_output_polarity(bool a, bool b) { ::pwm_config_set_output_polarity(&config_, a, b); return *this; }
		Config& set_chan_output_polarity(uint chan, bool inv) { set_chan_output_polarity(&config_, chan, inv); return *this; }
		Config& set_wrap(uint16_t wrap) { ::pwm_config_set_wrap(&config_, wrap); return *this; }
	public:
		static void set_chan_output_polarity(pwm_config *c, uint chan, bool inv) {
			c->csr = (chan == PWM_CHAN_A)?
				(c->csr & ~PWM_CH0_CSR_A_INV_BITS) | (bool_to_bit(inv) << PWM_CH0_CSR_A_INV_LSB) :
				(c->csr & ~PWM_CH0_CSR_B_INV_BITS) | (bool_to_bit(inv) << PWM_CH0_CSR_B_INV_LSB);
		}
	};
private:
	const GPIO& gpio_;
public:
	PWM(const GPIO& gpio) : gpio_{gpio} {}
public:
	const GPIO& GetGPIO() const { return gpio_; }
public:
	uint GetSliceNum() const { return ::pwm_gpio_to_slice_num(gpio_.pin); }
	uint GetChannel() const { return ::pwm_gpio_to_channel(gpio_.pin); }
	bool IsChannelA() const { return GetChannel() == PWM_CHAN_A; }
	bool IsChannelB() const { return GetChannel() == PWM_CHAN_B; }
public:
	const PWM& init(pwm_config *c, bool start) const { ::pwm_init(GetSliceNum(), c, start); return *this; }
	const PWM& init(Config& c, bool start) const { ::pwm_init(GetSliceNum(), c.GetEntityPtr(), start); return *this; }
	Config get_default_config() const { return Config(::pwm_get_default_config()); }
public:
	const PWM& set_phase_correct(bool phase_correct) const { ::pwm_set_phase_correct(GetSliceNum(), phase_correct); return *this; }
	const PWM& set_clkdiv(float divider) const { ::pwm_set_clkdiv(GetSliceNum(), divider); return *this; }
	const PWM& set_clkdiv_int_frac(uint8_t integer, uint8_t fract) const { ::pwm_set_clkdiv_int_frac(GetSliceNum(), integer, fract); return *this; }
	const PWM& set_clkdiv_mode(enum pwm_clkdiv_mode mode) const { ::pwm_set_clkdiv_mode(GetSliceNum(), mode); return *this; }
	const PWM& set_output_polarity(bool inv_a, bool inv_b) const { ::pwm_set_output_polarity(GetSliceNum(), inv_a, inv_b); return *this; }
	const PWM& set_chan_output_polarity(bool inv) const { set_chan_output_polarity(GetSliceNum(), GetChannel(), inv); return *this; }
	const PWM& set_wrap(uint16_t wrap) const { ::pwm_set_wrap(GetSliceNum(), wrap); return *this; }
public:
	//const PWM& set_chan_level(uint chan, uint16_t level) const { ::pwm_set_chan_level(GetSliceNum(), chan, level); return *this; }
	//const PWM& set_gpio_level(uint gpio, uint16_t level) const { ::pwm_set_gpio_level(gpio, level); return *this; }
	//const PWM& set_gpio_level(uint16_t level) const { ::pwm_set_gpio_level(gpio_.pin, level); return *this; }
public:
	const PWM& set_both_levels(uint16_t level_a, uint16_t level_b) const { ::pwm_set_both_levels(GetSliceNum(), level_a, level_b); return *this; }
	const PWM& set_chan_level(uint16_t level) const { ::pwm_set_gpio_level(gpio_.pin, level); return *this; }
	uint16_t get_counter() const { return ::pwm_get_counter(GetSliceNum()); }
	const PWM& set_counter(uint16_t c) const { ::pwm_set_counter(GetSliceNum(), c); return *this; }
	const PWM& advance_count() const { ::pwm_advance_count(GetSliceNum()); return *this; }
	const PWM& retard_count() const { ::pwm_retard_count(GetSliceNum()); return *this; }
	const PWM& set_enabled(bool enabled) const { ::pwm_set_enabled(GetSliceNum(), enabled); return *this; }
	static void set_mask_enabled(uint32_t mask) { ::pwm_set_mask_enabled(mask); }
public:
	const PWM& set_irq_enabled(bool enabled) const { ::pwm_set_irq_enabled(GetSliceNum(), enabled); return *this; }
	const PWM& set_irq0_enabled(bool enabled) const { ::pwm_set_irq0_enabled(GetSliceNum(), enabled); return *this; }
	const PWM& set_irqn_enabled(uint irq_index, bool enabled) const { ::pwm_irqn_set_slice_enabled(irq_index, GetSliceNum(), enabled); return *this; }
	const PWM& set_irq_mask_enabled(uint32_t slice_mask, bool enabled) const { ::pwm_set_irq_mask_enabled(slice_mask, enabled); return *this; }
	const PWM& set_irq0_mask_enabled(uint32_t slice_mask, bool enabled) const { ::pwm_set_irq0_mask_enabled(slice_mask, enabled); return *this; }
	const PWM& set_irqn_mask_enabled(uint irq_index, uint slice_mask, bool enabled) const { ::pwm_irqn_set_slice_mask_enabled(irq_index, slice_mask, enabled); return *this; }
	const PWM& clear_irq() const { ::pwm_clear_irq(GetSliceNum()); return *this; }
	static uint32_t get_irq_status_mask() { return ::pwm_get_irq_status_mask(); }
	static uint32_t get_irq0_status_mask() { return ::pwm_get_irq0_status_mask(); }
	static uint32_t get_irqn_status_mask(uint irq_index) { return ::pwm_irqn_get_status_mask(irq_index); }
	const PWM& force_irq() const { ::pwm_force_irq(GetSliceNum()); return *this; }
	const PWM& force_irq0() const { ::pwm_force_irq0(GetSliceNum()); return *this; }
	const PWM& force_irqn(uint irq_index) const { ::pwm_irqn_force(irq_index, GetSliceNum()); return *this; }
	uint get_dreq() const { return ::pwm_get_dreq(GetSliceNum()); }
public:
	static void set_chan_output_polarity(uint slice_num, uint chan, bool inv) {
		check_slice_num_param(slice_num);
		if (chan == PWM_CHAN_A) {
			hw_write_masked(&pwm_hw->slice[slice_num].csr, bool_to_bit(inv) << PWM_CH0_CSR_A_INV_LSB, PWM_CH0_CSR_A_INV_BITS);
		} else {
			hw_write_masked(&pwm_hw->slice[slice_num].csr, bool_to_bit(inv) << PWM_CH0_CSR_B_INV_LSB, PWM_CH0_CSR_B_INV_BITS);
		}
	}
};

}

#endif
