//==============================================================================
// jxglib/PIO.h
//==============================================================================
#ifndef PICO_JXGLIB_PIO_H
#define PICO_JXGLIB_PIO_H
#include <string.h>
#include <memory>
#include <cstddef>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "jxglib/Common.h"

#define PIOVAR_wrap_target(progName) 			progName##_wrap_target
#define PIOVAR_wrap(progName)					progName##_wrap
#define PIOVAR_pio_version(progName)			progName##_pio_version
#define PIOVAR_offset(progName, label)			progName##_offset_##label
#define PIOVAR_program(progName)				progName##_program
#define PIOVAR_get_default_config(progName)		progName##_program_get_default_config

using pio_t = PIO;

namespace jxglib {

namespace PIO {

//------------------------------------------------------------------------------
// PIO::Config
//------------------------------------------------------------------------------
class Config {
public:
	using T_get_default_config = pio_sm_config (*)(uint offset);
	class Generator {
	public:
		virtual pio_sm_config GenerateConfig(uint offset) const = 0;
	};
	class GeneratorPIOASM : public Generator {
	private:
		T_get_default_config get_default_config_;
	public:
		GeneratorPIOASM(T_get_default_config get_default_config) : get_default_config_{get_default_config} {}
	public:
		virtual pio_sm_config GenerateConfig(uint offset) const override { return get_default_config_(offset); }
	};
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

//------------------------------------------------------------------------------
// PIO::StateMachine
//------------------------------------------------------------------------------
class StateMachine {
private:
	pio_t pio_;
	uint sm_;
	uint offset_;
	const Config* pConfig_;
public:
	StateMachine() : pio_(nullptr), sm_{static_cast<uint>(-1)}, offset_{0}, pConfig_{nullptr} {}
	StateMachine(const StateMachine& stateMachine) : pio_(stateMachine.pio_), sm_{stateMachine.sm_} {}
public:
	operator uint() { return sm_; }
public:
	void SetResource(pio_t pio, uint sm, uint offset, const Config& config) { pio_ = pio, sm_ = sm, offset_ = offset, pConfig_ = &config; }
	void Invalidate() { pio_ = nullptr, sm_ = static_cast<uint>(-1); }	
	bool IsValid() const { return !!pio_ && sm_ != static_cast<uint>(-1); }
	pio_t GetPIO() const { return pio_; }
	uint GetSM() const { return sm_; }
	uint GetOffset() const { return offset_; }
	const Config& GetConfig() const { return *pConfig_; }
public:
	uint get_dreq(bool is_tx) const { return ::pio_get_dreq(pio_, sm_, is_tx); }
	uint get_dreq_tx() const { return ::pio_get_dreq(pio_, sm_, true); }
	uint get_dreq_rx() const { return ::pio_get_dreq(pio_, sm_, false); }
	int set_config(const pio_sm_config *config) const { return ::pio_sm_set_config(pio_, sm_, config); }
	int init(uint initial_pc, const pio_sm_config* config) { return ::pio_sm_init(pio_, sm_, initial_pc, config); }
	int init() { return ::pio_sm_init(pio_, sm_, offset_, *pConfig_); }
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

//------------------------------------------------------------------------------
// PIO::Instance
//------------------------------------------------------------------------------
class Instance {
public:
private:
	pio_t pio_;
public:
	Instance() : pio_{nullptr} {}
	Instance(pio_t pio) : pio_{pio} {}
	Instance(const Instance& pioIf) : pio_{pioIf.pio_} {}
public:
	operator pio_t() const { return pio_; }
public:
	void SetInst(pio_t pio) { pio_ = pio; }
	void Invalidate() { pio_ = nullptr; }
	bool IsValid() const { return !!pio_; }
public:
	uint get_gpio_base() const { return ::pio_get_gpio_base(pio_); }
	uint get_index() const { return ::pio_get_index(pio_); }
	uint get_funcsel() const { return ::pio_get_funcsel(pio_); }
	const Instance& gpio_init(uint pin) const { ::pio_gpio_init(pio_, pin); return *this; }
	int set_gpio_base(uint gpio_base) const { return ::pio_set_gpio_base(pio_, gpio_base); }
	bool can_add_program(const pio_program_t* program) const { return ::pio_can_add_program(pio_, program); }
	bool can_add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_can_add_program_at_offset(pio_, program, offset); }
	int add_program(const pio_program_t* program) const { return ::pio_add_program(pio_, program); }
	int add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_add_program_at_offset(pio_, program, offset); }
	const Instance& remove_program(const pio_program_t* program, uint loaded_offset) const { ::pio_remove_program(pio_, program, loaded_offset); return *this; }
	const Instance& clear_instruction_memory() const { ::pio_clear_instruction_memory(pio_); return *this; }
	const Instance& set_sm_mask_enabled(uint32_t mask, bool enabled) const { ::pio_set_sm_mask_enabled(pio_, mask, enabled); return *this; }
	const Instance& restart_sm_mask(uint32_t mask) const { ::pio_restart_sm_mask(pio_, mask); return *this; }
	const Instance& clkdiv_restart_sm_mask(uint32_t mask) const { ::pio_clkdiv_restart_sm_mask(pio_, mask); return *this; }
	const Instance& enable_sm_mask_in_sync(uint32_t mask) const { ::pio_enable_sm_mask_in_sync(pio_, mask); return *this; }
	const Instance& set_irq0_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq0_source_enabled(pio_, source, enabled); return *this; }
	const Instance& set_irq1_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq1_source_enabled(pio_, source, enabled); return *this; }
	const Instance& set_irq0_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq0_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	const Instance& set_irq1_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq1_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	const Instance& set_irqn_source_enabled(uint irq_index, pio_interrupt_source_t source, bool enabled) const { ::pio_set_irqn_source_enabled(pio_, irq_index, source, enabled); return *this; }
	const Instance& set_irqn_source_mask_enabled(uint irq_index, uint32_t source_mask, bool enabled) const { ::pio_set_irqn_source_mask_enabled(pio_, irq_index, source_mask, enabled); return *this; }
	bool interrupt_get(uint pio_interrupt_num) const { return ::pio_interrupt_get(pio_, pio_interrupt_num); }
	const Instance& interrupt_clear(uint pio_interrupt_num) const { ::pio_interrupt_clear(pio_, pio_interrupt_num); return *this; }
	const Instance& claim_sm_mask(uint sm_mask) const { ::pio_claim_sm_mask(pio_, sm_mask); return *this; }
	int claim_unused_sm(bool required) const { return ::pio_claim_unused_sm(pio_, required); }
	int get_irq_num(uint irqn) const { return ::pio_get_irq_num(pio_, irqn); }
public:	
	static pio_t get_instance(uint instance) { return ::pio_get_instance(instance); }
	static bool claim_free_sm_and_add_program(const pio_program_t* program, pio_t* pio, uint* sm, uint* offset) {
		return ::pio_claim_free_sm_and_add_program(program, pio, sm, offset);
	}
	static bool claim_free_sm_and_add_program_for_gpio_range(const pio_program_t* program, pio_t* pio, uint* sm, uint* offset, uint gpio_base, uint gpio_count, bool set_gpio_base) {
		return ::pio_claim_free_sm_and_add_program_for_gpio_range(program, pio, sm, offset, gpio_base, gpio_count, set_gpio_base);
	}
	static void remove_program_and_unclaim_sm(const pio_program_t* program, pio_t pio, uint sm, uint offset) {
		::pio_remove_program_and_unclaim_sm(program, pio, sm, offset);
	}
	static pio_interrupt_source_t get_tx_fifo_not_full_interrupt_source(uint sm) { return ::pio_get_tx_fifo_not_full_interrupt_source(sm); }
	static pio_interrupt_source_t get_rx_fifo_not_empty_interrupt_source(uint sm) { return ::pio_get_rx_fifo_not_empty_interrupt_source(sm); }
};

//------------------------------------------------------------------------------
// PIO::Program
//------------------------------------------------------------------------------
class Program : public Config::Generator {
public:
	class Variable {
		const char* label_;
		uint16_t value_;
		std::unique_ptr<Variable> pVariableNext_;
	public:
		Variable(const char* label, uint16_t addrRel) : label_{label}, value_{addrRel} {};
		const char* GetLabel() const { return label_; }
		uint16_t GetValue() const { return value_; }
		void SetNext(Variable* pVariableNext) { pVariableNext_.reset(pVariableNext); }
		Variable* GetNext() const { return pVariableNext_.get(); }
	};
	struct Wrap {
		uint16_t addrRel_target;
		uint16_t addrRel_wrap;
	};
	struct SideSet {
		uint bit_count;
		bool optional;
		bool pindirs;
	};
	enum class Directive {
		None,
		program,
		origin,
		pio_version,
		side_set,
		wrap_target,
		wrap,
		end,
	};
	static const uint16_t AddrRelInvalid = 0xffff;
private:
	const char* name_;
	uint16_t instTbl_[PIO_INSTRUCTION_COUNT];
	uint16_t addrRelCur_;
	Directive directive_;
	bool sideSpecifiedFlag_;
	Wrap wrap_;
	SideSet sideSet_;
	pio_program_t program_;
	std::unique_ptr<Variable> pVariableHead_;
	std::unique_ptr<Variable> pVariableRefHead_;
public:
	Program();
public:
	const char* GetName() const { return name_; }
	Program& AddInst(uint16_t inst);
	Program& L(const char* label);
public:
	bool IsSideMust() const { return sideSet_.bit_count > 0 && !sideSet_.optional; }
	const pio_program_t* GetProgram() const { return &program_; }
	void AddVariable(const char* label, uint16_t addrRel);
	void AddVariableRef(const char* label, uint16_t addrRel);
	const Variable* Lookup(const char* label) const;
	Program& Complete();
	const Program& Dump() const;
public:
	// virtual function of Config::Generator
	virtual pio_sm_config GenerateConfig(uint offset) const override;
public:
	// Directives
	Program& program(const char* name)	{ directive_ = Directive::program; name_ = name; return *this; }
	Program& origin(uint offset)		{ directive_ = Directive::origin; program_.origin = offset; return *this; }
	Program& pio_version(uint version)	{ directive_ = Directive::pio_version; program_.pio_version = version; return *this; }
	Program& side_set(int bit_count)	{ directive_ = Directive::side_set; sideSet_.bit_count = bit_count; return *this; }
	Program& wrap_target()				{ directive_ = Directive::wrap_target; wrap_.addrRel_target = addrRelCur_; return *this; }
	Program& wrap()						{ directive_ = Directive::wrap; wrap_.addrRel_wrap = addrRelCur_; return *this; }
	Program& end()						{ directive_ = Directive::end; return Complete(); }
public:
	Program& word(uint16_t inst) { return AddInst(inst); }
	Program& jmp(uint16_t addr) { return AddInst(::pio_encode_jmp(addr)); }
	Program& jmp_not_x(uint16_t addr) { return AddInst(::pio_encode_jmp_not_x(addr)); }
	Program& jmp_x_dec(uint16_t addr) { return AddInst(::pio_encode_jmp_x_dec(addr)); }
	Program& jmp_not_y(uint16_t addr) { return AddInst(::pio_encode_jmp_not_y(addr)); }
	Program& jmp_y_dec(uint16_t addr) { return AddInst(::pio_encode_jmp_y_dec(addr)); }
	Program& jmp_x_ne_y(uint16_t addr) { return AddInst(::pio_encode_jmp_x_ne_y(addr)); }
	Program& jmp_pin(uint16_t addr) { return AddInst(::pio_encode_jmp_pin(addr)); }
	Program& jmp_not_osre(uint16_t addr) { return AddInst(::pio_encode_jmp_not_osre(addr)); }
	Program& wait_gpio(bool polarity, uint16_t gpio) { return AddInst(::pio_encode_wait_gpio(polarity, gpio)); }
	Program& wait_pin(bool polarity, uint16_t pin) { return AddInst(::pio_encode_wait_pin(polarity, pin)); }
	Program& wait_irq(bool polarity, bool relative, uint16_t irq) { return AddInst(::pio_encode_wait_irq(polarity, relative, irq)); }
	Program& wait_jmppin(bool polarity, uint16_t pin_offset) { return AddInst(pio_encode_wait_jmppin(polarity, pin_offset)); }
	Program& in(enum pio_src_dest src, uint16_t count) { return AddInst(::pio_encode_in(src, count)); }
	Program& out(enum pio_src_dest dest, uint16_t count) { return AddInst(::pio_encode_out(dest, count)); }
	Program& push(bool if_full, bool block) { return AddInst(::pio_encode_push(if_full, block)); }
	Program& pull(bool if_empty, bool block) { return AddInst(::pio_encode_pull(if_empty, block)); }
	Program& mov(enum pio_src_dest dest, enum pio_src_dest src) { return AddInst(::pio_encode_mov(dest, src)); }
	Program& mov_not(enum pio_src_dest dest, enum pio_src_dest src) { return AddInst(::pio_encode_mov_not(dest, src)); }
	Program& mov_reverse(enum pio_src_dest dest, enum pio_src_dest src) { return AddInst(::pio_encode_mov_reverse(dest, src)); }
	Program& irq_set(uint16_t irq_n, bool relative = false) { return AddInst(::pio_encode_irq_set(relative, irq_n)); }
	Program& irq_wait(uint16_t irq_n, bool relative = false) { return AddInst(::pio_encode_irq_wait(relative, irq_n)); }
	Program& irq_clear(uint16_t irq_n, bool relative = false) { return AddInst(::pio_encode_irq_clear(relative, irq_n)); }
	Program& set(enum pio_src_dest dest, uint16_t value) { return AddInst(::pio_encode_set(dest, value)); }
	Program& nop() { return AddInst(::pio_encode_nop()); }
private:
	static uint pio_encode_wait_jmppin(bool polarity, uint pin_offset) {
		valid_params_if(PIO_INSTRUCTIONS, pin_offset <= 3);
		return _pio_encode_instr_and_args(pio_instr_bits_wait, 3u | (polarity ? 4u : 0u), pin_offset);
	}
public:
	// Assembler-style instructions
	Program& jmp(const char* label) { AddVariableRef(label, addrRelCur_); return jmp(static_cast<uint16_t>(0)); }
	Program& jmp(const char* cond, uint16_t addr);
	Program& jmp(const char* cond, const char* label) { AddVariableRef(label, addrRelCur_); return jmp(cond, static_cast<uint16_t>(0)); }
	Program& wait(bool polarity, const char* src, uint16_t index);
	Program& in(const char* src, uint16_t count) { return in(StrToSrcDest(src), count); }
	Program& out(const char* dest, uint16_t count) { return out(StrToSrcDest_out(dest), count); }
	Program& push() { return push(false, false); }
	Program& pull() { return pull(false, false); }
	Program& mov(const char* dest, const char* src, uint16_t index = 0);
	Program& mov(const char* dest, uint16_t index, const char* src) { return mov(dest, src, index); }
	Program& irq(const char* op, uint16_t irq_n);
	Program& irq(uint16_t irq_n) { return irq("set", irq_n); }
	Program& set(const char* dest, uint16_t value) { return set(StrToSrcDest(dest), value); }
public:
	// Attributes
	Program& opt();
	Program& pindirs();
	Program& side(uint16_t bits);
	Program& delay(uint16_t cycles);
	Program& operator[](uint16_t cycles) { return delay(cycles); }
	Program& rel();
	Program& prev();
	Program& next();
	Program& iffull();
	Program& ifempty();
	Program& block();
	Program& noblock();
public:
	pio_src_dest StrToSrcDest(const char* str, bool outFlag = false) const;
	pio_src_dest StrToSrcDest_out(const char* str) const { return StrToSrcDest(str, true); }
public:
	static bool Is_JMP(uint16_t inst)		{ return (inst >> 13) == 0; }
	static bool Is_WAIT(uint16_t inst)		{ return (inst >> 13) == 1; }
	static bool Is_IN(uint16_t inst)		{ return (inst >> 13) == 2; }
	static bool Is_OUT(uint16_t inst)		{ return (inst >> 13) == 3; }
	static bool Is_PUSHorPULL(uint16_t inst){ return (inst >> 13) == 4 && (inst & 0b00010000) == 0b00000000; }
	static bool Is_PUSH(uint16_t inst)		{ return (inst >> 13) == 4 && (inst & 0b10010000) == 0b00000000; }
	static bool Is_PULL(uint16_t inst)		{ return (inst >> 13) == 4 && (inst & 0b10010000) == 0b10000000; }
	static bool Is_MOVtoRX(uint16_t inst)	{ return (inst >> 13) == 4 && (inst & 0b10010000) == 0b00010000; }
	static bool Is_MOVfromRX(uint16_t inst)	{ return (inst >> 13) == 4 && (inst & 0b10010000) == 0b10010000; }
	static bool Is_MOV(uint16_t inst)		{ return (inst >> 13) == 5; }
	static bool Is_IRQ(uint16_t inst)		{ return (inst >> 13) == 6; }
	static bool Is_SET(uint16_t inst)		{ return (inst >> 13) == 7; }
	static const char* GetInstName(uint16_t inst);
};

//------------------------------------------------------------------------------
// PIO::Controller
//------------------------------------------------------------------------------
class Controller {
public:
	struct GPIOInfo {
		const GPIO* pGPIO;
		int cnt;
	};
private:
	const pio_program_t* program;
	const Config::Generator& configGenerator_;
	const Config::GeneratorPIOASM configGeneratorPIOASM_;
	Instance pio_;
	Config config_;
public:
	StateMachine sm;
private:
	GPIOInfo gpioInfo_out;
	GPIOInfo gpioInfo_in;
	GPIOInfo gpioInfo_set;
	GPIOInfo gpioInfo_sideset;
public:
	Controller(const pio_program_t& program, Config::T_get_default_config get_default_config) :
		program{&program}, configGenerator_{configGeneratorPIOASM_}, configGeneratorPIOASM_(get_default_config) {}
	Controller(const Program& program) :
		program{program.GetProgram()}, configGenerator_{program}, configGeneratorPIOASM_(nullptr) {}
public:
	bool IsValid() { return sm.IsValid(); }
	Controller& Set_out(const GPIO& gpio, int cnt = 1) {
		gpioInfo_out.pGPIO = &gpio, gpioInfo_out.cnt = cnt; return *this;
	};
	Controller& Set_in(const GPIO& gpio, int cnt = 1) {
		gpioInfo_in.pGPIO = &gpio, gpioInfo_in.cnt = cnt; return *this;
	};
	Controller& Set_set(const GPIO& gpio, int cnt = 1) {
		gpioInfo_set.pGPIO = &gpio, gpioInfo_set.cnt = cnt; return *this;
	};
	Controller& Set_sideset(const GPIO& gpio, int cnt = 1) {
		gpioInfo_sideset.pGPIO = &gpio, gpioInfo_sideset.cnt = cnt; return *this;
	};
	bool ClaimResource();
	void UnclaimResource();
	int InitSM();
};

}

extern PIO::Instance PIO0;
extern PIO::Instance PIO1;

}

#endif
