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
		constexpr Config() : config_{0} {}
		constexpr Config(const pwm_config& config) : config_{config} {}
		constexpr Config(const Config& config) : config_{config.config_} {}
	public:
		pwm_config& GetEntity() { return config_; }
		const pwm_config& GetEntity() const { return config_; }
		pwm_config* GetEntityPtr() { return &config_; }
		const pwm_config* GetEntityPtr() const { return &config_; }
	public:
		bool get_phase_correct() const;
		Config& set_phase_correct(bool phase_correct) { ::pwm_config_set_phase_correct(&config_, phase_correct); return *this; }
		Config& set_freq(uint32_t freq);
		Config& set_clkdiv(float div) { ::pwm_config_set_clkdiv(&config_, div); return *this; }
		Config& set_clkdiv_int_frac(uint8_t div_int, uint8_t div_frac) { ::pwm_config_set_clkdiv_int_frac(&config_, div_int, div_frac); return *this; }
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
	uint pin_;
public:
	PWM(uint pin) : pin_{pin} {}
public:
	uint GetPin() const { return pin_; }
public:
	uint GetSliceNum() const { return ::pwm_gpio_to_slice_num(pin_); }
	uint GetChannel() const { return ::pwm_gpio_to_channel(pin_); }
	uint32_t GetSliceMask() const { return (1u << GetSliceNum()); }
	bool IsChannelA() const { return GetChannel() == PWM_CHAN_A; }
	bool IsChannelB() const { return GetChannel() == PWM_CHAN_B; }
public:
	const PWM& set_freq(uint32_t freq) const;
	uint32_t get_freq() const { return CalcFreq(get_phase_correct(), get_clkdiv(), get_wrap()); }
	const PWM& set_chan_duty(float duty) const { set_chan_duty(GetSliceNum(), GetChannel(), duty); return *this; }
	float get_chan_duty() const { return get_chan_duty(GetSliceNum(), GetChannel()); }
public:
	const PWM& init(pwm_config *c, bool start) const { ::pwm_init(GetSliceNum(), c, start); return *this; }
	const PWM& init(Config& c, bool start) const { ::pwm_init(GetSliceNum(), c.GetEntityPtr(), start); return *this; }
	static Config get_default_config() { return Config(::pwm_get_default_config()); }
public:
	bool get_phase_correct() const { return get_phase_correct(GetSliceNum()); }
	const PWM& set_phase_correct(bool phase_correct = true) const { ::pwm_set_phase_correct(GetSliceNum(), phase_correct); return *this; }
	const PWM& set_clkdiv(float div) const { ::pwm_set_clkdiv(GetSliceNum(), div); return *this; }
	float get_clkdiv() const { return get_clkdiv(GetSliceNum()); }
	const PWM& set_clkdiv_int_frac(uint8_t div_int, uint8_t div_frac) const { ::pwm_set_clkdiv_int_frac(GetSliceNum(), div_int, div_frac); return *this; }
	const PWM& set_clkdiv_mode(enum pwm_clkdiv_mode mode) const { ::pwm_set_clkdiv_mode(GetSliceNum(), mode); return *this; }
	const PWM& set_output_polarity(bool inv_a, bool inv_b) const { ::pwm_set_output_polarity(GetSliceNum(), inv_a, inv_b); return *this; }
	const PWM& set_chan_output_polarity(bool inv) const { set_chan_output_polarity(GetSliceNum(), GetChannel(), inv); return *this; }
	const PWM& set_wrap(uint16_t wrap) const { ::pwm_set_wrap(GetSliceNum(), wrap); return *this; }
	uint16_t get_wrap() const { return get_wrap(GetSliceNum()); }
public:
	const PWM& set_both_levels(uint16_t level_a, uint16_t level_b) const { ::pwm_set_both_levels(GetSliceNum(), level_a, level_b); return *this; }
	uint16_t get_chan_level() const { return get_chan_level(GetSliceNum(), GetChannel()); }
	const PWM& set_chan_level(uint16_t level) const { ::pwm_set_chan_level(GetSliceNum(), GetChannel(), level); return *this; }
	uint16_t get_counter() const { return ::pwm_get_counter(GetSliceNum()); }
	const PWM& set_counter(uint16_t c) const { ::pwm_set_counter(GetSliceNum(), c); return *this; }
	const PWM& advance_count() const { ::pwm_advance_count(GetSliceNum()); return *this; }
	const PWM& retard_count() const { ::pwm_retard_count(GetSliceNum()); return *this; }
	const PWM& set_enabled(bool enabled) const { ::pwm_set_enabled(GetSliceNum(), enabled); return *this; }
	bool is_enabled() const { return is_enabled(GetSliceNum()); }
	static void set_mask_enabled(uint32_t mask) { ::pwm_set_mask_enabled(mask); }
	static void enable(const PWM& pwm) {
		::pwm_set_mask_enabled(get_mask_enabled() | pwm.GetSliceMask());
	}
	static void enable(const PWM& pwm1, const PWM& pwm2) {
		::pwm_set_mask_enabled(get_mask_enabled() | pwm1.GetSliceMask() | pwm2.GetSliceMask());
	}
	static void enable(const PWM& pwm1, const PWM& pwm2, const PWM& pwm3) {
		::pwm_set_mask_enabled(get_mask_enabled() | pwm1.GetSliceMask() | pwm2.GetSliceMask() | pwm3.GetSliceMask());
	}
	static void enable(const PWM& pwm1, const PWM& pwm2, const PWM& pwm3, const PWM& pwm4) {
		::pwm_set_mask_enabled(get_mask_enabled() | pwm1.GetSliceMask() | pwm2.GetSliceMask() | pwm3.GetSliceMask() | pwm4.GetSliceMask());
	}
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
	static void CalcClkdivAndWrap(bool phaseCorrect, uint32_t freq, float* pClkdiv, uint16_t* pWrap);
	static uint32_t CalcFreq(bool phaseCorrect, float clkdiv, uint16_t wrap);
	static uint32_t CalcFreq(bool phaseCorrect, uint8_t div_int, uint8_t div_frac, uint16_t wrap);
public:
	static uint32_t get_mask_enabled();
	static bool get_phase_correct(uint slice_num);
	static float get_clkdiv(uint slice_num);
	static uint16_t get_wrap(uint slice_num);
	static uint16_t get_chan_level(uint slice_num, uint channel);
	static float get_chan_duty(uint slice_num, uint channel);
	static void set_chan_duty(uint slice_num, uint channel, float duty);
	static void set_chan_output_polarity(uint slice_num, uint chan, bool inv);
	static bool is_enabled(uint slice_num);
};

}

#endif
