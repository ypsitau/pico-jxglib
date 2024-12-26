//==============================================================================
// jxglib/Interp.h
//==============================================================================
#ifndef PICO_JXGLIB_INTERP_H
#define PICO_JXGLIB_INTERP_H
#include "pico/stdlib.h"
#include "hardware/interp.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Interp
//------------------------------------------------------------------------------
class Interp {
public:
	class Config {
	private:
		interp_config config_;
	public:
		Config(const interp_config& config) : config_{config} {}
	public:
		operator interp_config*() { return &config_; }
		operator interp_config*() const { return const_cast<interp_config*>(&config_); }
	public:
		Config& set_shift(uint shift) { ::interp_config_set_shift(&config_, shift); return *this; }
		Config& set_mask(uint mask_lsb, uint mask_msb) { ::interp_config_set_mask(&config_, mask_lsb, mask_msb); return *this; }
		Config& set_cross_input(bool cross_input = true) { ::interp_config_set_cross_input(&config_, cross_input); return *this; }
		Config& set_cross_result(bool cross_result = true) { ::interp_config_set_cross_result(&config_, cross_result); return *this; }
		Config& set_signed(bool _signed = true) { ::interp_config_set_signed(&config_, _signed); return *this; }
		Config& set_add_raw(bool add_raw = true) { ::interp_config_set_add_raw(&config_, add_raw); return *this; }
		Config& set_blend(bool blend = true) { ::interp_config_set_blend(&config_, blend); return *this; }
		Config& set_clamp(bool clamp = true) { ::interp_config_set_clamp(&config_, clamp); return *this; }
		Config& set_force_bits(uint bits) { ::interp_config_set_force_bits(&config_, bits); return *this; }
	};
	class Saver {
	private:
		interp_hw_save_t saver_;
	public:
		Saver(const interp_hw_save_t& saver) : saver_{saver} {}
	public:
		operator interp_hw_save_t*() { return &saver_; }
		operator interp_hw_save_t*() const { return const_cast<interp_hw_save_t*>(&saver_); }
	};
private:
	interp_hw_t* interp_;
public:
	Interp(interp_hw_t* interp) : interp_{interp} {}
public:
	interp_hw_t* operator->() { return interp_; }
	operator interp_hw_t*() { return interp_; }
	operator interp_hw_t*() const { return interp_; }
public:
	Interp& claim_lane(uint lane) { ::interp_claim_lane(interp_, lane); return *this; }
	Interp& claim_lane_mask(uint lane_mask) { ::interp_claim_lane_mask(interp_, lane_mask); return *this; }
	Interp& unclaim_lane(uint lane) { ::interp_unclaim_lane(interp_, lane); return *this; }
	bool lane_is_claimed(uint lane) { return ::interp_lane_is_claimed(interp_, lane); }
	Interp& unclaim_lane_mask(uint lane_mask) { ::interp_unclaim_lane_mask(interp_, lane_mask); return *this; }
	void set_force_bits(uint lane, uint bits) { return ::interp_set_force_bits(interp_, lane, bits); }
	Interp& save(interp_hw_save_t* saver) { ::interp_save(interp_, saver); return *this; }
	Interp& save(Saver& saver) { interp_hw_save_t saver_c; ::interp_save(interp_, &saver_c); saver = saver_c; return *this; }
	Interp& restore(interp_hw_save_t* saver) { ::interp_restore(interp_, saver); return *this; }
	Interp& set_base(uint lane, uint32_t val) { ::interp_set_base(interp_, lane, val); return *this; }
	uint32_t get_base(uint lane) { return ::interp_get_base(interp_, lane); }
	Interp& set_base_both(uint32_t val) { ::interp_set_base_both(interp_, val); return *this; }
	Interp& set_accumulator(uint lane, uint32_t val) { ::interp_set_accumulator(interp_, lane, val); return *this; }
	uint32_t get_accumulator(uint lane) { return ::interp_get_accumulator(interp_, lane); }
	uint32_t pop_lane_result(uint lane) { return ::interp_pop_lane_result(interp_, lane); }
	uint32_t peek_lane_result(uint lane) { return ::interp_peek_lane_result(interp_, lane); }
	uint32_t pop_full_result(interp_hw_t* interp) { return ::interp_pop_full_result(interp_); }
	uint32_t peek_full_result(interp_hw_t* interp) { return ::interp_peek_full_result(interp_); }
	Interp& add_accumulater(uint lane, uint32_t val) { ::interp_add_accumulater(interp_, lane, val); return *this; }
	uint32_t get_raw(uint lane) { return ::interp_get_raw(interp_, lane); }
public:
	Interp& set_config(uint lane, interp_config* config) { ::interp_set_config(interp_, lane, config); return *this; }
	static Config default_config(void) { return Config(::interp_default_config()); }
};

extern Interp Interp0;
extern Interp Interp1;

}

#endif
