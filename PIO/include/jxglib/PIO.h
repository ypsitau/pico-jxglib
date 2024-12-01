//==============================================================================
// jxglib/PIO.h
//==============================================================================
#ifndef PICO_JXGLIB_PIO_H
#define PICO_JXGLIB_PIO_H
#include "pico/stdlib.h"
#include "hardware/pio.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PIOx
//------------------------------------------------------------------------------
class PIOx {
public:
	class SMConfig {
	private:
		pio_sm_config c_;
	public:
		SMConfig(const pio_sm_config& c) : c_{c} {}
	public:
		SMConfig& set_out_pin_base(uint out_base) { ::sm_config_set_out_pin_base(&c_, out_base); return *this; }
		SMConfig& set_out_pin_count(uint out_count) { ::sm_config_set_out_pin_count(&c_, out_count); return *this; }
		SMConfig& set_out_pins(uint out_base, uint out_count) { ::sm_config_set_out_pins(&c_, out_base, out_count); return *this; }
		SMConfig& set_set_pin_base(uint set_base) { ::sm_config_set_set_pin_base(&c_, set_base); return *this; }
		SMConfig& set_set_pin_count(uint set_count) { ::sm_config_set_set_pin_count(&c_, set_count); return *this; }
		SMConfig& set_set_pins(uint set_base, uint set_count) { ::sm_config_set_set_pins(&c_, set_base, set_count); return *this; }
		SMConfig& set_in_pin_base(uint in_base) { ::sm_config_set_in_pin_base(&c_, in_base); return *this; }
		SMConfig& set_in_pins(uint in_base) { ::sm_config_set_in_pins(&c_, in_base); return *this; }
		SMConfig& set_sideset_pin_base(uint sideset_base) { ::sm_config_set_sideset_pin_base(&c_, sideset_base); return *this; }
		SMConfig& set_sideset_pins(uint sideset_base) { ::sm_config_set_sideset_pins(&c_, sideset_base); return *this; }
		SMConfig& set_sideset(uint bit_count, bool optional, bool pindirs) { ::sm_config_set_sideset(&c_, bit_count, optional, pindirs); return *this; }
		SMConfig& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) { ::sm_config_set_clkdiv_int_frac(&c_, div_int, div_frac); return *this; }
		SMConfig& set_clkdiv(float div) { ::sm_config_set_clkdiv(&c_, div); return *this; }
		SMConfig& set_wrap(uint wrap_target, uint wrap) { ::sm_config_set_wrap(&c_, wrap_target, wrap); return *this; }
		SMConfig& set_jmp_pin(uint pin) { ::sm_config_set_jmp_pin(&c_, pin); return *this; }
		SMConfig& set_in_shift(bool shift_right, bool autopush, uint push_threshold) { ::sm_config_set_in_shift(&c_, shift_right, autopush, push_threshold); return *this; }
		SMConfig& set_out_shift(bool shift_right, bool autopull, uint pull_threshold) { ::sm_config_set_out_shift(&c_, shift_right, autopull, pull_threshold); return *this; }
		SMConfig& set_fifo_join(enum pio_fifo_join join) { ::sm_config_set_fifo_join(&c_, join); return *this; }
		SMConfig& set_out_special(bool sticky, bool has_enable_pin, uint enable_pin_index) { ::sm_config_set_out_special(&c_, sticky, has_enable_pin, enable_pin_index); return *this; }
		SMConfig& set_mov_status(enum pio_mov_status_type status_sel, uint status_n) { ::sm_config_set_mov_status(&c_, status_sel, status_n); return *this; }
	};
	class StateMachine {
	private:
		PIO pio_;
		uint sm_;
	public:
		StateMachine(PIO pio, uint sm) : pio_{pio}, sm_{sm} {}
	public:
		operator uint() { return sm_; }
	public:
		uint get_dreq(bool is_tx) { return ::pio_get_dreq(pio_, sm_, is_tx); }
		uint get_dreq_tx() { return ::pio_get_dreq(pio_, sm_, true); }
		uint get_dreq_rx() { return ::pio_get_dreq(pio_, sm_, false); }
		int set_config(const pio_sm_config *config) { return ::pio_sm_set_config(pio_, sm_, config); }
		StateMachine& set_enabled(bool enabled) { ::pio_sm_set_enabled(pio_, sm_, enabled); return *this; }
		StateMachine& restart() { ::pio_sm_restart(pio_, sm_); return *this; }
		StateMachine& clkdiv_restart() { ::pio_sm_clkdiv_restart(pio_, sm_); return *this; }
		uint8_t get_pc() {return  ::pio_sm_get_pc(pio_, sm_); }
		StateMachine& exec(uint instr) { ::pio_sm_exec(pio_, sm_, instr); return *this; }
		bool is_exec_stalled() { return ::pio_sm_is_exec_stalled(pio_, sm_); }
		StateMachine& exec_wait_blocking(uint instr) { ::pio_sm_exec_wait_blocking(pio_, sm_, instr); return *this; }
		StateMachine& set_wrap(uint wrap_target, uint wrap) { ::pio_sm_set_wrap(pio_, sm_, wrap_target, wrap); return *this; }
		StateMachine& set_out_pins(uint out_base, uint out_count) { ::pio_sm_set_out_pins(pio_, sm_, out_base, out_count); return *this; }
		StateMachine& set_set_pins(uint set_base, uint set_count) { ::pio_sm_set_set_pins(pio_, sm_, set_base, set_count); return *this; }
		StateMachine& set_in_pins(uint in_base) { ::pio_sm_set_in_pins(pio_, sm_, in_base); return *this; }
		StateMachine& set_sideset_pins(uint sideset_base) { ::pio_sm_set_sideset_pins(pio_, sm_, sideset_base); return *this; }
		StateMachine& set_jmp_pin(uint pin) { ::pio_sm_set_jmp_pin(pio_, sm_, pin); return *this; }
		StateMachine& put(uint32_t data) { ::pio_sm_put(pio_, sm_, data); return *this; }
		uint32_t get() { return ::pio_sm_get(pio_, sm_); }
		bool is_rx_fifo_full() { return ::pio_sm_is_rx_fifo_full(pio_, sm_); }
		bool is_rx_fifo_empty() { return ::pio_sm_is_rx_fifo_empty(pio_, sm_); }
		uint get_rx_fifo_level() { return ::pio_sm_get_rx_fifo_level(pio_, sm_); }
		bool is_tx_fifo_full() { return ::pio_sm_is_tx_fifo_full(pio_, sm_); }
		bool is_tx_fifo_empty() { return ::pio_sm_is_tx_fifo_empty(pio_, sm_); }
		uint get_tx_fifo_level() { return ::pio_sm_get_tx_fifo_level(pio_, sm_); }
		StateMachine& put_blocking(uint32_t data) { ::pio_sm_put_blocking(pio_, sm_, data); return *this; }
		uint32_t get_blocking() { return ::pio_sm_get_blocking(pio_, sm_); }
		StateMachine& drain_tx_fifo() { ::pio_sm_drain_tx_fifo(pio_, sm_); return *this; }
		StateMachine& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) { ::pio_sm_set_clkdiv_int_frac(pio_, sm_, div_int, div_frac); return *this; }
		StateMachine& set_clkdiv(float div) { ::pio_sm_set_clkdiv(pio_, sm_, div); return *this; }
		StateMachine& clear_fifos() { ::pio_sm_clear_fifos(pio_, sm_); return *this; }
		StateMachine& set_pins(uint32_t pin_values) { ::pio_sm_set_pins(pio_, sm_, pin_values); return *this; }
		StateMachine& set_pins_with_mask(uint32_t pin_values, uint32_t pin_mask) { ::pio_sm_set_pins_with_mask(pio_, sm_, pin_values, pin_mask); return *this; }
		StateMachine& set_pindirs_with_mask(uint32_t pin_dirs, uint32_t pin_mask) { ::pio_sm_set_pindirs_with_mask(pio_, sm_, pin_dirs, pin_mask); return *this; }
		int set_consecutive_pindirs(uint pins_base, uint pin_count, bool is_out) { return ::pio_sm_set_consecutive_pindirs(pio_, sm_, pins_base, pin_count, is_out); }
		StateMachine& claim() { ::pio_sm_claim(pio_, sm_); return *this; }
		StateMachine& unclaim() { ::pio_sm_unclaim(pio_, sm_); return *this; }
		bool is_claimed() { return ::pio_sm_is_claimed(pio_, sm_); }
	};
private:
	PIO pio_;
public:
	StateMachine SM0;
	StateMachine SM1;
	StateMachine SM2;
	StateMachine SM3;
private:
	StateMachine* stateMachineTbl_[4];
public:
	PIOx(PIO pio);		
public:
	operator PIO() { return pio_; }
public:
	uint get_gpio_base() { return ::pio_get_gpio_base(pio_); }
	uint get_index() { return ::pio_get_index(pio_); }
	uint get_funcsel() { return ::pio_get_funcsel(pio_); }
	PIOx& gpio_init(uint pin) { ::pio_gpio_init(pio_, pin); return *this; }
	int set_gpio_base(uint gpio_base) { return ::pio_set_gpio_base(pio_, gpio_base); }
	bool can_add_program(const pio_program_t *program) { return ::pio_can_add_program(pio_, program); }
	bool can_add_program_at_offset(const pio_program_t *program, uint offset) { return ::pio_can_add_program_at_offset(pio_, program, offset); }
	int add_program(const pio_program_t *program) { return ::pio_add_program(pio_, program); }
	int add_program_at_offset(const pio_program_t *program, uint offset) { return ::pio_add_program_at_offset(pio_, program, offset); }
	PIOx& remove_program(const pio_program_t *program, uint loaded_offset) { ::pio_remove_program(pio_, program, loaded_offset); return *this; }
	PIOx& clear_instruction_memory() { ::pio_clear_instruction_memory(pio_); return *this; }
	PIOx& set_sm_mask_enabled(uint32_t mask, bool enabled) { ::pio_set_sm_mask_enabled(pio_, mask, enabled); return *this; }
	PIOx& restart_sm_mask(uint32_t mask) { ::pio_restart_sm_mask(pio_, mask); return *this; }
	PIOx& clkdiv_restart_sm_mask(uint32_t mask) { ::pio_clkdiv_restart_sm_mask(pio_, mask); return *this; }
	PIOx& enable_sm_mask_in_sync(uint32_t mask) { ::pio_enable_sm_mask_in_sync(pio_, mask); return *this; }
	PIOx& set_irq0_source_enabled(pio_interrupt_source_t source, bool enabled) { ::pio_set_irq0_source_enabled(pio_, source, enabled); return *this; }
	PIOx& set_irq1_source_enabled(pio_interrupt_source_t source, bool enabled) { ::pio_set_irq1_source_enabled(pio_, source, enabled); return *this; }
	PIOx& set_irq0_source_mask_enabled(uint32_t source_mask, bool enabled) { ::pio_set_irq0_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	PIOx& set_irq1_source_mask_enabled(uint32_t source_mask, bool enabled) { ::pio_set_irq1_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	PIOx& set_irqn_source_enabled(uint irq_index, pio_interrupt_source_t source, bool enabled) { ::pio_set_irqn_source_enabled(pio_, irq_index, source, enabled); return *this; }
	PIOx& set_irqn_source_mask_enabled(uint irq_index, uint32_t source_mask, bool enabled) { ::pio_set_irqn_source_mask_enabled(pio_, irq_index, source_mask, enabled); return *this; }
	bool interrupt_get(uint pio_interrupt_num) { return ::pio_interrupt_get(pio_, pio_interrupt_num); }
	PIOx& interrupt_clear(uint pio_interrupt_num) { ::pio_interrupt_clear(pio_, pio_interrupt_num); return *this; }
	PIOx& claim_sm_mask(uint sm_mask) { ::pio_claim_sm_mask(pio_, sm_mask); return *this; }
	int claim_unused_sm(bool required) { return ::pio_claim_unused_sm(pio_, required); }
	int get_irq_num(uint irqn) { return ::pio_get_irq_num(pio_, irqn); }
public:	
	static SMConfig pio_get_default_sm_config(void) { return SMConfig(::pio_get_default_sm_config()); }
	static PIO get_instance(uint instance) { return ::pio_get_instance(instance); }
	static bool claim_free_sm_and_add_program(const pio_program_t *program, PIO *pio, uint *sm, uint *offset) {
		return ::pio_claim_free_sm_and_add_program(program, pio, sm, offset);
	}
	static bool claim_free_sm_and_add_program_for_gpio_range(const pio_program_t *program, PIO *pio, uint *sm, uint *offset, uint gpio_base, uint gpio_count, bool set_gpio_base) {
		return ::pio_claim_free_sm_and_add_program_for_gpio_range(program, pio, sm, offset, gpio_base, gpio_count, set_gpio_base);
	}
	static void remove_program_and_unclaim_sm(const pio_program_t *program, PIO pio, uint sm, uint offset) {
		::pio_remove_program_and_unclaim_sm(program, pio, sm, offset);
	}
	static pio_interrupt_source_t get_tx_fifo_not_full_interrupt_source(uint sm) { return ::pio_get_tx_fifo_not_full_interrupt_source(sm); }
	static pio_interrupt_source_t get_rx_fifo_not_empty_interrupt_source(uint sm) { return ::pio_get_rx_fifo_not_empty_interrupt_source(sm); }
};

extern PIOx PIO0;
extern PIOx PIO1;

}

#endif
