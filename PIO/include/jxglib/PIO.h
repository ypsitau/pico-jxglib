//==============================================================================
// jxglib/PIO.h
//==============================================================================
#ifndef PICO_JXGLIB_PIO_H
#define PICO_JXGLIB_PIO_H
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "jxglib/GPIO.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PIOIf
//------------------------------------------------------------------------------
class PIOIf {
public:
	class StateMachine {
	private:
		PIO pio_;
		uint sm_;
	public:
		StateMachine() : pio_(nullptr), sm_{static_cast<uint>(-1)} {}
		StateMachine(const StateMachine& stateMachine) : pio_(stateMachine.pio_), sm_{stateMachine.sm_} {}
	public:
		operator uint() { return sm_; }
	public:
		void SetResource(PIO pio, uint sm) { pio_ = pio, sm_ = sm; }
		void Invalidate() { pio_ = nullptr, sm_ = static_cast<uint>(-1); }	
		bool IsValid() const { return !!pio_ && sm_ != static_cast<uint>(-1); }
		PIO GetPIO() const { return pio_; }
		uint GetSM() const { return sm_; }
	public:
		uint get_dreq(bool is_tx) const { return ::pio_get_dreq(pio_, sm_, is_tx); }
		uint get_dreq_tx() const { return ::pio_get_dreq(pio_, sm_, true); }
		uint get_dreq_rx() const { return ::pio_get_dreq(pio_, sm_, false); }
		int set_config(const pio_sm_config *config) const { return ::pio_sm_set_config(pio_, sm_, config); }
		int init(uint initial_pc, const pio_sm_config* config) { return ::pio_sm_init(pio_, sm_, initial_pc, config); }
		const StateMachine& set_enabled(bool enabled) const { ::pio_sm_set_enabled(pio_, sm_, enabled); return *this; }
		const StateMachine& restart() const { ::pio_sm_restart(pio_, sm_); return *this; }
		const StateMachine& clkdiv_restart() const { ::pio_sm_clkdiv_restart(pio_, sm_); return *this; }
		uint8_t get_pc() const {return  ::pio_sm_get_pc(pio_, sm_); }
		const StateMachine& exec(uint instr) const { ::pio_sm_exec(pio_, sm_, instr); return *this; }
		bool is_exec_stalled() const { return ::pio_sm_is_exec_stalled(pio_, sm_); }
		const StateMachine& exec_wait_blocking(uint instr) const { ::pio_sm_exec_wait_blocking(pio_, sm_, instr); return *this; }
		const StateMachine& set_wrap(uint wrap_target, uint wrap) const { ::pio_sm_set_wrap(pio_, sm_, wrap_target, wrap); return *this; }
		const StateMachine& set_out_pins(uint out_base, uint out_count) const { ::pio_sm_set_out_pins(pio_, sm_, out_base, out_count); return *this; }
		const StateMachine& set_set_pins(uint set_base, uint set_count) const { ::pio_sm_set_set_pins(pio_, sm_, set_base, set_count); return *this; }
		const StateMachine& set_in_pins(uint in_base) const { ::pio_sm_set_in_pins(pio_, sm_, in_base); return *this; }
		const StateMachine& set_sideset_pins(uint sideset_base) const { ::pio_sm_set_sideset_pins(pio_, sm_, sideset_base); return *this; }
		const StateMachine& set_jmp_pin(uint pin) const { ::pio_sm_set_jmp_pin(pio_, sm_, pin); return *this; }
		const StateMachine& put(uint32_t data) const { ::pio_sm_put(pio_, sm_, data); return *this; }
		uint32_t get() const { return ::pio_sm_get(pio_, sm_); }
		bool is_rx_fifo_full() const { return ::pio_sm_is_rx_fifo_full(pio_, sm_); }
		bool is_rx_fifo_empty() const { return ::pio_sm_is_rx_fifo_empty(pio_, sm_); }
		uint get_rx_fifo_level() const { return ::pio_sm_get_rx_fifo_level(pio_, sm_); }
		bool is_tx_fifo_full() const { return ::pio_sm_is_tx_fifo_full(pio_, sm_); }
		bool is_tx_fifo_empty() const { return ::pio_sm_is_tx_fifo_empty(pio_, sm_); }
		uint get_tx_fifo_level() const { return ::pio_sm_get_tx_fifo_level(pio_, sm_); }
		const StateMachine& put_blocking(uint32_t data) const { ::pio_sm_put_blocking(pio_, sm_, data); return *this; }
		uint32_t get_blocking() const { return ::pio_sm_get_blocking(pio_, sm_); }
		const StateMachine& drain_tx_fifo() const { ::pio_sm_drain_tx_fifo(pio_, sm_); return *this; }
		const StateMachine& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) const { ::pio_sm_set_clkdiv_int_frac(pio_, sm_, div_int, div_frac); return *this; }
		const StateMachine& set_clkdiv(float div) const { ::pio_sm_set_clkdiv(pio_, sm_, div); return *this; }
		const StateMachine& clear_fifos() const { ::pio_sm_clear_fifos(pio_, sm_); return *this; }
		const StateMachine& set_pins(uint32_t pin_values) const { ::pio_sm_set_pins(pio_, sm_, pin_values); return *this; }
		const StateMachine& set_pins_with_mask(uint32_t pin_values, uint32_t pin_mask) const { ::pio_sm_set_pins_with_mask(pio_, sm_, pin_values, pin_mask); return *this; }
		const StateMachine& set_pindirs_with_mask(uint32_t pin_dirs, uint32_t pin_mask) const { ::pio_sm_set_pindirs_with_mask(pio_, sm_, pin_dirs, pin_mask); return *this; }
		int set_consecutive_pindirs(uint pins_base, uint pin_count, bool is_out) const { return ::pio_sm_set_consecutive_pindirs(pio_, sm_, pins_base, pin_count, is_out); }
		int set_consecutive_pindirs_out(uint pins_base, uint pin_count) const { return ::pio_sm_set_consecutive_pindirs(pio_, sm_, pins_base, pin_count, true); }
		int set_consecutive_pindirs_in(uint pins_base, uint pin_count) const { return ::pio_sm_set_consecutive_pindirs(pio_, sm_, pins_base, pin_count, false); }
		const StateMachine& claim() const { ::pio_sm_claim(pio_, sm_); return *this; }
		const StateMachine& unclaim() const { ::pio_sm_unclaim(pio_, sm_); return *this; }
		bool is_claimed() const { return ::pio_sm_is_claimed(pio_, sm_); }
	};
	class Config {
	private:
		pio_sm_config c_;
	public:
		Config() : c_{::pio_get_default_sm_config()} {}
		Config(const pio_sm_config& c) : c_{c} {}
	public:
		operator const pio_sm_config*() const { return &c_; }
	public:
		Config& set_out_pin_base(uint out_base) { ::sm_config_set_out_pin_base(&c_, out_base); return *this; }
		Config& set_out_pin_count(uint out_count) { ::sm_config_set_out_pin_count(&c_, out_count); return *this; }
		Config& set_out_pins(uint out_base, uint out_count) { ::sm_config_set_out_pins(&c_, out_base, out_count); return *this; }
		Config& set_set_pin_base(uint set_base) { ::sm_config_set_set_pin_base(&c_, set_base); return *this; }
		Config& set_set_pin_count(uint set_count) { ::sm_config_set_set_pin_count(&c_, set_count); return *this; }
		Config& set_set_pins(uint set_base, uint set_count) { ::sm_config_set_set_pins(&c_, set_base, set_count); return *this; }
		Config& set_in_pin_base(uint in_base) { ::sm_config_set_in_pin_base(&c_, in_base); return *this; }
		Config& set_in_pins(uint in_base) { ::sm_config_set_in_pins(&c_, in_base); return *this; }
		Config& set_sideset_pin_base(uint sideset_base) { ::sm_config_set_sideset_pin_base(&c_, sideset_base); return *this; }
		Config& set_sideset_pins(uint sideset_base) { ::sm_config_set_sideset_pins(&c_, sideset_base); return *this; }
		Config& set_sideset(uint bit_count, bool optional, bool pindirs) { ::sm_config_set_sideset(&c_, bit_count, optional, pindirs); return *this; }
		Config& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) { ::sm_config_set_clkdiv_int_frac(&c_, div_int, div_frac); return *this; }
		Config& set_clkdiv(float div) { ::sm_config_set_clkdiv(&c_, div); return *this; }
		Config& set_wrap(uint wrap_target, uint wrap) { ::sm_config_set_wrap(&c_, wrap_target, wrap); return *this; }
		Config& set_jmp_pin(uint pin) { ::sm_config_set_jmp_pin(&c_, pin); return *this; }
		Config& set_in_shift(bool shift_right, bool autopush, uint push_threshold) { ::sm_config_set_in_shift(&c_, shift_right, autopush, push_threshold); return *this; }
		Config& set_out_shift(bool shift_right, bool autopull, uint pull_threshold) { ::sm_config_set_out_shift(&c_, shift_right, autopull, pull_threshold); return *this; }
		Config& set_fifo_join(enum pio_fifo_join join) { ::sm_config_set_fifo_join(&c_, join); return *this; }
		Config& set_out_special(bool sticky, bool has_enable_pin, uint enable_pin_index) { ::sm_config_set_out_special(&c_, sticky, has_enable_pin, enable_pin_index); return *this; }
		Config& set_mov_status(enum pio_mov_status_type status_sel, uint status_n) { ::sm_config_set_mov_status(&c_, status_sel, status_n); return *this; }
	};
private:
	PIO pio_;
public:
	PIOIf() : pio_{nullptr} {}
	PIOIf(PIO pio) : pio_{pio} {}
	PIOIf(const PIOIf& pioIf) : pio_{pioIf.pio_} {}
public:
	operator PIO() const { return pio_; }
public:
	void SetInst(PIO pio) { pio_ = pio; }
	void Invalidate() { pio_ = nullptr; }
	bool IsValid() const { return !!pio_; }
public:
	uint get_gpio_base() const { return ::pio_get_gpio_base(pio_); }
	uint get_index() const { return ::pio_get_index(pio_); }
	uint get_funcsel() const { return ::pio_get_funcsel(pio_); }
	const PIOIf& gpio_init(uint pin) const { ::pio_gpio_init(pio_, pin); return *this; }
	int set_gpio_base(uint gpio_base) const { return ::pio_set_gpio_base(pio_, gpio_base); }
	bool can_add_program(const pio_program_t* program) const { return ::pio_can_add_program(pio_, program); }
	bool can_add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_can_add_program_at_offset(pio_, program, offset); }
	int add_program(const pio_program_t* program) const { return ::pio_add_program(pio_, program); }
	int add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_add_program_at_offset(pio_, program, offset); }
	const PIOIf& remove_program(const pio_program_t* program, uint loaded_offset) const { ::pio_remove_program(pio_, program, loaded_offset); return *this; }
	const PIOIf& clear_instruction_memory() const { ::pio_clear_instruction_memory(pio_); return *this; }
	const PIOIf& set_sm_mask_enabled(uint32_t mask, bool enabled) const { ::pio_set_sm_mask_enabled(pio_, mask, enabled); return *this; }
	const PIOIf& restart_sm_mask(uint32_t mask) const { ::pio_restart_sm_mask(pio_, mask); return *this; }
	const PIOIf& clkdiv_restart_sm_mask(uint32_t mask) const { ::pio_clkdiv_restart_sm_mask(pio_, mask); return *this; }
	const PIOIf& enable_sm_mask_in_sync(uint32_t mask) const { ::pio_enable_sm_mask_in_sync(pio_, mask); return *this; }
	const PIOIf& set_irq0_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq0_source_enabled(pio_, source, enabled); return *this; }
	const PIOIf& set_irq1_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq1_source_enabled(pio_, source, enabled); return *this; }
	const PIOIf& set_irq0_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq0_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	const PIOIf& set_irq1_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq1_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	const PIOIf& set_irqn_source_enabled(uint irq_index, pio_interrupt_source_t source, bool enabled) const { ::pio_set_irqn_source_enabled(pio_, irq_index, source, enabled); return *this; }
	const PIOIf& set_irqn_source_mask_enabled(uint irq_index, uint32_t source_mask, bool enabled) const { ::pio_set_irqn_source_mask_enabled(pio_, irq_index, source_mask, enabled); return *this; }
	bool interrupt_get(uint pio_interrupt_num) const { return ::pio_interrupt_get(pio_, pio_interrupt_num); }
	const PIOIf& interrupt_clear(uint pio_interrupt_num) const { ::pio_interrupt_clear(pio_, pio_interrupt_num); return *this; }
	const PIOIf& claim_sm_mask(uint sm_mask) const { ::pio_claim_sm_mask(pio_, sm_mask); return *this; }
	int claim_unused_sm(bool required) const { return ::pio_claim_unused_sm(pio_, required); }
	int get_irq_num(uint irqn) const { return ::pio_get_irq_num(pio_, irqn); }
public:	
	static PIO get_instance(uint instance) { return ::pio_get_instance(instance); }
	static bool claim_free_sm_and_add_program(const pio_program_t* program, PIO* pio, uint* sm, uint* offset) {
		return ::pio_claim_free_sm_and_add_program(program, pio, sm, offset);
	}
	static bool claim_free_sm_and_add_program_for_gpio_range(const pio_program_t* program, PIO* pio, uint* sm, uint* offset, uint gpio_base, uint gpio_count, bool set_gpio_base) {
		return ::pio_claim_free_sm_and_add_program_for_gpio_range(program, pio, sm, offset, gpio_base, gpio_count, set_gpio_base);
	}
	static void remove_program_and_unclaim_sm(const pio_program_t* program, PIO pio, uint sm, uint offset) {
		::pio_remove_program_and_unclaim_sm(program, pio, sm, offset);
	}
	static pio_interrupt_source_t get_tx_fifo_not_full_interrupt_source(uint sm) { return ::pio_get_tx_fifo_not_full_interrupt_source(sm); }
	static pio_interrupt_source_t get_rx_fifo_not_empty_interrupt_source(uint sm) { return ::pio_get_rx_fifo_not_empty_interrupt_source(sm); }
};

struct PIOEncode {
public:
	static uint delay(uint cycles) { return ::pio_encode_delay(cycles); }
	static uint sideset(uint sideset_bit_count, uint value) { return ::pio_encode_sideset(sideset_bit_count, value); }
	static uint sideset_opt(uint sideset_bit_count, uint value) { return ::pio_encode_sideset_opt(sideset_bit_count, value); }
	static uint jmp(uint addr) { return ::pio_encode_jmp(addr); }
	static uint jmp_not_x(uint addr) { return ::pio_encode_jmp_not_x(addr); }
	static uint jmp_x_dec(uint addr) { return ::pio_encode_jmp_x_dec(addr); }
	static uint jmp_not_y(uint addr) { return ::pio_encode_jmp_not_y(addr); }
	static uint jmp_y_dec(uint addr) { return ::pio_encode_jmp_y_dec(addr); }
	static uint jmp_x_ne_y(uint addr) { return ::pio_encode_jmp_x_ne_y(addr); }
	static uint jmp_pin(uint addr) { return ::pio_encode_jmp_pin(addr); }
	static uint jmp_not_osre(uint addr) { return ::pio_encode_jmp_not_osre(addr); }
	static uint wait_gpio(bool polarity, uint gpio) { return ::pio_encode_wait_gpio(polarity, gpio); }
	static uint wait_pin(bool polarity, uint pin) { return ::pio_encode_wait_pin(polarity, pin); }
	static uint wait_irq(bool polarity, bool relative, uint irq) { return ::pio_encode_wait_irq(polarity, relative, irq); }
	static uint in(enum pio_src_dest src, uint count) { return ::pio_encode_in(src, count); }
	static uint out(enum pio_src_dest dest, uint count) { return ::pio_encode_out(dest, count); }
	static uint push(bool if_full, bool block) { return ::pio_encode_push(if_full, block); }
	static uint pull(bool if_empty, bool block) { return ::pio_encode_pull(if_empty, block); }
	static uint mov(enum pio_src_dest dest, enum pio_src_dest src) { return ::pio_encode_mov(dest, src); }
	static uint mov_not(enum pio_src_dest dest, enum pio_src_dest src) { return ::pio_encode_mov_not(dest, src); }
	static uint mov_reverse(enum pio_src_dest dest, enum pio_src_dest src) { return ::pio_encode_mov_reverse(dest, src); }
	static uint irq_set(bool relative, uint irq) { return ::pio_encode_irq_set(relative, irq); }
	static uint irq_wait(bool relative, uint irq) { return ::pio_encode_irq_wait(relative, irq); }
	static uint irq_clear(bool relative, uint irq) { return ::pio_encode_irq_clear(relative, irq); }
	static uint set(enum pio_src_dest dest, uint value) { return ::pio_encode_set(dest, value); }
	static uint nop(void) { return ::pio_encode_nop(); }
};

class PIOContext {
public:
	const pio_program_t* program;
	PIOIf::StateMachine sm;
	PIOIf pio;
	uint offset;
public:
	PIOContext(const pio_program_t& program) : program{&program}, offset{static_cast<uint>(-1)} {}
public:
	bool IsValid() { return sm.IsValid(); }
	bool ClaimResource();
	bool ClaimResource(uint gpio_base, uint gpio_count, bool set_gpio_base);
	void UnclaimResource();
	int Init(const PIOIf::Config& cfg);
};

extern PIOIf PIO0;
extern PIOIf PIO1;

}

#endif
