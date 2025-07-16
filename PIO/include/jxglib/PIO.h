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
#include "hardware/clocks.h"
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

inline void CheckAdjacent(uint pin1, uint pin2) { if (pin2 != pin1 + 1) ::panic("pins must be adjacent\n"); }
inline void CheckAdjacent(uint pin1, uint pin2, uint pin3) { if (pin3 != pin2 + 1 || pin2 != pin1 + 1) ::panic("pins must be adjacent\n"); }
inline void CheckAdjacent(uint pin1, uint pin2, uint pin3, uint pin4) { if (pin4 != pin3 + 1 || pin3 != pin2 + 1 || pin2 != pin1 + 1) ::panic("pins must be adjacent\n"); }

//------------------------------------------------------------------------------
// PIO::Config
//------------------------------------------------------------------------------
class Config {
public:
	using T_get_default_config = pio_sm_config (*)(uint offset);
private:
	pio_sm_config c_;
public:
	Config() : c_{::pio_get_default_sm_config()} {}
	Config(const pio_sm_config& c) : c_{c} {}
	Config(const Config& config) : c_{config.c_} {}
public:
	operator pio_sm_config*() { return &c_; }
	operator const pio_sm_config*() const { return &c_; }
public:
	Config& set_out_pin_base(uint out_base) { ::sm_config_set_out_pin_base(&c_, out_base); return *this; }
	Config& set_out_pin_count(uint out_count) { ::sm_config_set_out_pin_count(&c_, out_count); return *this; }
	Config& set_out_pins(uint out_base, uint out_count) { ::sm_config_set_out_pins(&c_, out_base, out_count); return *this; }
	Config& set_out_pin(uint pin) { return set_out_pins(pin, 1); }
	Config& set_out_pin(uint pin1, uint pin2) { CheckAdjacent(pin1, pin2); return set_out_pins(pin1, 2); }
	Config& set_out_pin(uint pin1, uint pin2, uint pin3) { CheckAdjacent(pin1, pin2, pin3); return set_out_pins(pin1, 3); }
	Config& set_out_pin(uint pin1, uint pin2, uint pin3, uint pin4) { CheckAdjacent(pin1, pin2, pin3, pin4); return set_out_pins(pin1, 4); }
public:
	Config& set_set_pin_base(uint set_base) { ::sm_config_set_set_pin_base(&c_, set_base); return *this; }
	Config& set_set_pin_count(uint set_count) { ::sm_config_set_set_pin_count(&c_, set_count); return *this; }
	Config& set_set_pins(uint set_base, uint set_count) { ::sm_config_set_set_pins(&c_, set_base, set_count); return *this; }
	Config& set_set_pin(uint pin) { return set_set_pins(pin, 1); }
	Config& set_set_pin(uint pin1, uint pin2) { CheckAdjacent(pin1, pin2); return set_set_pins(pin1, 2); }
	Config& set_set_pin(uint pin1, uint pin2, uint pin3) { CheckAdjacent(pin1, pin2, pin3); return set_set_pins(pin1, 3); }
	Config& set_set_pin(uint pin1, uint pin2, uint pin3, uint pin4) { CheckAdjacent(pin1, pin2, pin3, pin4); return set_set_pins(pin1, 4); }
public:
	Config& set_in_pin_base(uint in_base) { ::sm_config_set_in_pin_base(&c_, in_base); return *this; }
	Config& set_in_pins(uint in_base) { ::sm_config_set_in_pins(&c_, in_base); return *this; }
	Config& set_in_pin(uint pin) { return set_in_pins(pin); }
	Config& set_in_pin(uint pin1, uint pin2) { CheckAdjacent(pin1, pin2); return set_in_pins(pin1); }
	Config& set_in_pin(uint pin1, uint pin2, uint pin3) { CheckAdjacent(pin1, pin2, pin3); return set_in_pins(pin1); }
	Config& set_in_pin(uint pin1, uint pin2, uint pin3, uint pin4) { CheckAdjacent(pin1, pin2, pin3, pin4); return set_in_pins(pin1); }
public:
	Config& set_sideset_pin_base(uint sideset_base) { ::sm_config_set_sideset_pin_base(&c_, sideset_base); return *this; }
	Config& set_sideset_pins(uint sideset_base) { ::sm_config_set_sideset_pins(&c_, sideset_base); return *this; }
	Config& set_sideset_pin(uint pin) { return set_sideset_pins(pin); }
	Config& set_sideset_pin(uint pin1, uint pin2) { CheckAdjacent(pin1, pin2); return set_sideset_pins(pin1); }
	Config& set_sideset_pin(uint pin1, uint pin2, uint pin3) { CheckAdjacent(pin1, pin2, pin3); return set_sideset_pins(pin1); }
	Config& set_sideset_pin(uint pin1, uint pin2, uint pin3, uint pin4) { CheckAdjacent(pin1, pin2, pin3, pin4); return set_sideset_pins(pin1); }
public:
	Config& set_jmp_pin(uint pin) { ::sm_config_set_jmp_pin(&c_, pin); return *this; }
public:
	Config& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) { ::sm_config_set_clkdiv_int_frac(&c_, div_int, div_frac); return *this; }
	Config& set_clkdiv(float div) { ::sm_config_set_clkdiv(&c_, div); return *this; }
public:
	Config& set_sideset(uint bit_count, bool optional, bool pindirs) { ::sm_config_set_sideset(&c_, bit_count, optional, pindirs); return *this; }
	Config& set_wrap(uint wrap_target, uint wrap) { ::sm_config_set_wrap(&c_, wrap_target, wrap); return *this; }
public:
	Config& set_in_shift(bool shift_right, bool autopush = false, uint push_threshold = 32) { ::sm_config_set_in_shift(&c_, shift_right, autopush, push_threshold); return *this; }
	Config& set_in_shift_left(bool autopush = false, uint push_threshold = 32) { return set_in_shift(false, autopush, push_threshold); }
	Config& set_in_shift_right(bool autopush = false, uint push_threshold = 32) { return set_in_shift(true, autopush, push_threshold); }
public:
	Config& set_out_shift(bool shift_right, bool autopull = false, uint pull_threshold = 32) { ::sm_config_set_out_shift(&c_, shift_right, autopull, pull_threshold); return *this; }
	Config& set_out_shift_left(bool autopull = false, uint pull_threshold = 32) { return set_out_shift(false, autopull, pull_threshold); }
	Config& set_out_shift_right(bool autopull = false, uint pull_threshold = 32) { return set_out_shift(true, autopull, pull_threshold); }
public:
	Config& set_fifo_join(enum pio_fifo_join join) { ::sm_config_set_fifo_join(&c_, join); return *this; }
	Config& set_fifo_join_none() { return set_fifo_join(PIO_FIFO_JOIN_NONE); }
	Config& set_fifo_join_tx() { return set_fifo_join(PIO_FIFO_JOIN_TX); }
	Config& set_fifo_join_rx() { return set_fifo_join(PIO_FIFO_JOIN_RX); }
#if PICO_PIO_VERSION > 0
	Config& set_fifo_join_txput() { return set_fifo_join(PIO_FIFO_JOIN_TXPUT); }
	Config& set_fifo_join_txget() { return set_fifo_join(PIO_FIFO_JOIN_TXGET); }
	Config& set_fifo_join_putget() { return set_fifo_join(PIO_FIFO_JOIN_PUTGET); }
#endif
public:
	Config& set_out_special(bool sticky, bool has_enable_pin, uint enable_pin_index) { ::sm_config_set_out_special(&c_, sticky, has_enable_pin, enable_pin_index); return *this; }
	Config& set_mov_status(enum pio_mov_status_type status_sel, uint status_n) { ::sm_config_set_mov_status(&c_, status_sel, status_n); return *this; }
};

//------------------------------------------------------------------------------
// PIO::Program
//------------------------------------------------------------------------------
class Program {
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
		uint16_t addrRel_wrapPlus;
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
	static const Program None;
public:
	Program();
public:
	void Reset();
	const char* GetName() const { return name_; }
	Program& AddInst(uint16_t inst);
	Program& L(const char* label, uint* pAddrRel = nullptr);
public:
	bool IsSideMust() const { return sideSet_.bit_count > 0 && !sideSet_.optional; }
	void AddVariable(const char* label, uint16_t addrRel);
	void AddVariableRef(const char* label, uint16_t addrRel);
	const Variable* LookupVariable(const char* label) const;
	Program& Complete();
	void Configure(pio_sm_config* config, uint offset) const;
	const Program& Dump() const;
public:
	operator const pio_program_t*() const { return &program_; }
public:
	// Directives
	Program& program(const char* name)	{ Reset(); directive_ = Directive::program; name_ = name; return *this; }
	Program& origin(uint offset)		{ directive_ = Directive::origin; program_.origin = offset; return *this; }
	Program& pio_version(uint version)	{ directive_ = Directive::pio_version; program_.pio_version = version; return *this; }
	Program& side_set(int bit_count);
	Program& wrap_target(uint* pAddrRel = nullptr);
	Program& wrap(uint* pAddrRel = nullptr);
	Program& end(uint* pAddrRel = nullptr);
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
// PIO::Block
//------------------------------------------------------------------------------
class Block {
private:
	pio_t pio_;
public:
	Block() : pio_{nullptr} {}
	Block(pio_t pio) : pio_{pio} {}
	Block(const Block& pioIf) : pio_{pioIf.pio_} {}
public:
	operator pio_t() const { return pio_; }
	pio_t operator->() { return pio_; }
public:
	void Invalidate() { pio_ = nullptr; }
	bool IsValid() const { return !!pio_; }
public:
	const Block& gpio_init(uint pin) const { ::pio_gpio_init(pio_, pin); return *this; }
	uint get_gpio_base() const { return ::pio_get_gpio_base(pio_); }
	uint get_index() const { return ::pio_get_index(pio_); }
	uint get_funcsel() const { return ::pio_get_funcsel(pio_); }
	int set_gpio_base(uint gpio_base) const { return ::pio_set_gpio_base(pio_, gpio_base); }
public:
	bool can_add_program(const pio_program_t* program) const { return ::pio_can_add_program(pio_, program); }
	bool can_add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_can_add_program_at_offset(pio_, program, offset); }
	int add_program(const pio_program_t* program) const { return ::pio_add_program(pio_, program); }
	int add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_add_program_at_offset(pio_, program, offset); }
	const Block& remove_program(const pio_program_t* program, uint loaded_offset) const { ::pio_remove_program(pio_, program, loaded_offset); return *this; }
	const Block& clear_instruction_memory() const { ::pio_clear_instruction_memory(pio_); return *this; }
public:
	const Block& set_sm_mask_enabled(uint32_t mask, bool enabled) const { ::pio_set_sm_mask_enabled(pio_, mask, enabled); return *this; }
	const Block& restart_sm_mask(uint32_t mask) const { ::pio_restart_sm_mask(pio_, mask); return *this; }
	const Block& clkdiv_restart_sm_mask(uint32_t mask) const { ::pio_clkdiv_restart_sm_mask(pio_, mask); return *this; }
	const Block& enable_sm_mask_in_sync(uint32_t mask) const { ::pio_enable_sm_mask_in_sync(pio_, mask); return *this; }
public:
	const Block& set_irq0_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq0_source_enabled(pio_, source, enabled); return *this; }
	const Block& set_irq1_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq1_source_enabled(pio_, source, enabled); return *this; }
	const Block& set_irq0_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq0_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	const Block& set_irq1_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq1_source_mask_enabled(pio_, source_mask, enabled); return *this; }
	const Block& set_irqn_source_enabled(uint irq_index, pio_interrupt_source_t source, bool enabled) const { ::pio_set_irqn_source_enabled(pio_, irq_index, source, enabled); return *this; }
	const Block& set_irqn_source_mask_enabled(uint irq_index, uint32_t source_mask, bool enabled) const { ::pio_set_irqn_source_mask_enabled(pio_, irq_index, source_mask, enabled); return *this; }
	bool interrupt_get(uint pio_interrupt_num) const { return ::pio_interrupt_get(pio_, pio_interrupt_num); }
	const Block& interrupt_clear(uint pio_interrupt_num) const { ::pio_interrupt_clear(pio_, pio_interrupt_num); return *this; }
	int get_irq_num(uint irqn) const { return ::pio_get_irq_num(pio_, irqn); }
public:
	const Block& claim_sm_mask(uint sm_mask) const { ::pio_claim_sm_mask(pio_, sm_mask); return *this; }
	int claim_unused_sm(bool required) const { return ::pio_claim_unused_sm(pio_, required); }
public:	
	static pio_t get_instance(uint instance) { return ::pio_get_instance(instance); }
};

//------------------------------------------------------------------------------
// PIO::StateMachine
//------------------------------------------------------------------------------
class StateMachine {
public:
	const Program& program;
	Block pio;
	uint sm;
	uint offset;
	Config config;
public:
	StateMachine(const Program& program, pio_t pio = nullptr, uint sm = static_cast<uint>(-1), uint offset = 0) : program{program}, pio{pio}, sm{sm}, offset{offset} {}
	StateMachine(const StateMachine& stateMachine) : program{stateMachine.program}, pio{stateMachine.pio}, sm{stateMachine.sm}, offset{stateMachine.offset}, config{stateMachine.config} {}
public:
	operator uint() { return sm; }
public:
	bool IsValid() const { return !!pio && sm != static_cast<uint>(-1); }
	void Invalidate() { pio = nullptr, sm = static_cast<uint>(-1), offset = 0; }
public:
	void SetResource(pio_t pio, uint sm);
	void ClaimResource();
	void ClaimResource(StateMachine& smToShareProgram);
	void ClaimResource(uint gpio_base, uint gpio_count, bool set_gpio_base);
	void UnclaimResource();
public:
	const StateMachine& set_out_pins(uint out_base, uint out_count) const { ::pio_sm_set_out_pins(pio, sm, out_base, out_count); return *this; }
	const StateMachine& set_out_pin(uint pin) const { return set_out_pins(pin, 1); }
	const StateMachine& set_out_pin(uint pin1, uint pin2) const { CheckAdjacent(pin1, pin2); return set_out_pins(pin1, 2); }
	const StateMachine& set_out_pin(uint pin1, uint pin2, uint pin3) const { CheckAdjacent(pin1, pin2, pin3); return set_out_pins(pin1, 3); }
	const StateMachine& set_out_pin(uint pin1, uint pin2, uint pin3, uint pin4) const { CheckAdjacent(pin1, pin2, pin3, pin4); return set_out_pins(pin1, 4); }
public:
	const StateMachine& set_set_pins(uint set_base, uint set_count) const { ::pio_sm_set_set_pins(pio, sm, set_base, set_count); return *this; }
	const StateMachine& set_set_pin(uint pin) const { return set_set_pins(pin, 1); }
	const StateMachine& set_set_pin(uint pin1, uint pin2) const { CheckAdjacent(pin1, pin2); return set_set_pins(pin1, 2); }
	const StateMachine& set_set_pin(uint pin1, uint pin2, uint pin3) const { CheckAdjacent(pin1, pin2, pin3); return set_set_pins(pin1, 3); }
	const StateMachine& set_set_pin(uint pin1, uint pin2, uint pin3, uint pin4) const { CheckAdjacent(pin1, pin2, pin3, pin4); return set_set_pins(pin1, 4); }
public:
	const StateMachine& set_in_pins(uint in_base) const { ::pio_sm_set_in_pins(pio, sm, in_base); return *this; }
	const StateMachine& set_in_pin(uint pin) const { return set_in_pins(pin); }
	const StateMachine& set_in_pin(uint pin1, uint pin2) const { CheckAdjacent(pin1, pin2); return set_in_pins(pin1); }
	const StateMachine& set_in_pin(uint pin1, uint pin2, uint pin3) const { CheckAdjacent(pin1, pin2, pin3); return set_in_pins(pin1); }
	const StateMachine& set_in_pin(uint pin1, uint pin2, uint pin3, uint pin4) const { CheckAdjacent(pin1, pin2, pin3, pin4); return set_in_pins(pin1); }
public:
	const StateMachine& set_sideset_pins(uint sideset_base) const { ::pio_sm_set_sideset_pins(pio, sm, sideset_base); return *this; }
	const StateMachine& set_sideset_pin(uint pin) const { return set_sideset_pins(pin); }
	const StateMachine& set_sideset_pin(uint pin1, uint pin2) const { CheckAdjacent(pin1, pin2); return set_sideset_pins(pin1); }
	const StateMachine& set_sideset_pin(uint pin1, uint pin2, uint pin3) const { CheckAdjacent(pin1, pin2, pin3); return set_sideset_pins(pin1); }
	const StateMachine& set_sideset_pin(uint pin1, uint pin2, uint pin3, uint pin4) const { CheckAdjacent(pin1, pin2, pin3, pin4); return set_sideset_pins(pin1); }
public:
	const StateMachine& set_jmp_pin(uint pin) const { ::pio_sm_set_jmp_pin(pio, sm, pin); return *this; }
public:
	const StateMachine& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) const { ::pio_sm_set_clkdiv_int_frac(pio, sm, div_int, div_frac); return *this; }
	const StateMachine& set_clkdiv(float div) const { ::pio_sm_set_clkdiv(pio, sm, div); return *this; }
	const StateMachine& set_wrap(uint wrap_target, uint wrap) const { ::pio_sm_set_wrap(pio, sm, wrap_target, wrap); return *this; }
public:
	int init(uint addrRel_entry, const pio_sm_config* config) { return ::pio_sm_init(pio, sm, offset + addrRel_entry, config); }
	int init() { return ::pio_sm_init(pio, sm, offset, config); }
	int set_config(const pio_sm_config *config) const { return ::pio_sm_set_config(pio, sm, config); }
	const StateMachine& set_enabled(bool enabled = true) const { ::pio_sm_set_enabled(pio, sm, enabled); return *this; }
	const StateMachine& restart() const { ::pio_sm_restart(pio, sm); return *this; }
	const StateMachine& clkdiv_restart() const { ::pio_sm_clkdiv_restart(pio, sm); return *this; }
	const StateMachine& exec(uint instr) const { ::pio_sm_exec(pio, sm, instr); return *this; }
	bool is_exec_stalled() const { return ::pio_sm_is_exec_stalled(pio, sm); }
	const StateMachine& exec_wait_blocking(uint instr) const { ::pio_sm_exec_wait_blocking(pio, sm, instr); return *this; }
public:
	int set_consecutive_pindirs(uint pins_base, uint pin_count, bool is_out) const { return ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, is_out); }
	int set_pindirs(uint pins_base, uint pin_count, bool is_out) const { return ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, is_out); }
	int set_pindirs_out(uint pins_base, uint pin_count) const { return ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, true); }
	int set_pindirs_in(uint pins_base, uint pin_count) const { return ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, false); }
	int set_pindir(uint pin, bool is_out) const { return set_pindirs(pin, 1, is_out); }
	int set_pindir_out(uint pin) const { return set_pindirs(pin, 1, true); }
	int set_pindir_out(uint pin1, uint pin2) const { CheckAdjacent(pin1, pin2); return set_pindirs(pin1, 2, true); }
	int set_pindir_out(uint pin1, uint pin2, uint pin3) const { CheckAdjacent(pin1, pin2, pin3); return set_pindirs(pin1, 3, true); }
	int set_pindir_out(uint pin1, uint pin2, uint pin3, uint pin4) const { CheckAdjacent(pin1, pin2, pin3, pin4); return set_pindirs(pin1, 4, true); }
	int set_pindir_in(uint pin) const { return set_pindirs(pin, 1, false); }
	int set_pindir_in(uint pin1, uint pin2) const { CheckAdjacent(pin1, pin2); return set_pindirs(pin1, 2, false); }
	int set_pindir_in(uint pin1, uint pin2, uint pin3) const { CheckAdjacent(pin1, pin2, pin3); return set_pindirs(pin1, 3, false); }
	int set_pindir_in(uint pin1, uint pin2, uint pin3, uint pin4) const { CheckAdjacent(pin1, pin2, pin3, pin4); return set_pindirs(pin1, 4, false); }
public:
	uint get_dreq(bool is_tx) const { return ::pio_get_dreq(pio, sm, is_tx); }
	uint get_dreq_tx() const { return ::pio_get_dreq(pio, sm, true); }
	uint get_dreq_rx() const { return ::pio_get_dreq(pio, sm, false); }
	uint8_t get_pc() const { return  ::pio_sm_get_pc(pio, sm); }
public:
	bool is_rx_fifo_full() const { return ::pio_sm_is_rx_fifo_full(pio, sm); }
	bool is_rx_fifo_empty() const { return ::pio_sm_is_rx_fifo_empty(pio, sm); }
	uint get_rx_fifo_level() const { return ::pio_sm_get_rx_fifo_level(pio, sm); }
	bool is_tx_fifo_full() const { return ::pio_sm_is_tx_fifo_full(pio, sm); }
	bool is_tx_fifo_empty() const { return ::pio_sm_is_tx_fifo_empty(pio, sm); }
	uint get_tx_fifo_level() const { return ::pio_sm_get_tx_fifo_level(pio, sm); }
	const StateMachine& drain_tx_fifo() const { ::pio_sm_drain_tx_fifo(pio, sm); return *this; }
	const StateMachine& clear_fifos() const { ::pio_sm_clear_fifos(pio, sm); return *this; }
	pio_interrupt_source_t get_tx_fifo_not_full_interrupt_source() { return ::pio_get_tx_fifo_not_full_interrupt_source(sm); }
	pio_interrupt_source_t get_rx_fifo_not_empty_interrupt_source() { return ::pio_get_rx_fifo_not_empty_interrupt_source(sm); }
public:
	const StateMachine& put(uint32_t data) const { ::pio_sm_put(pio, sm, data); return *this; }
	uint32_t get() const { return ::pio_sm_get(pio, sm); }
	const StateMachine& put_blocking(uint32_t data) const { ::pio_sm_put_blocking(pio, sm, data); return *this; }
	uint32_t get_blocking() const { return ::pio_sm_get_blocking(pio, sm); }
public:
	const StateMachine& set_pins(uint32_t pin_values) const { ::pio_sm_set_pins(pio, sm, pin_values); return *this; }
	const StateMachine& set_pins_with_mask(uint32_t pin_values, uint32_t pin_mask) const { ::pio_sm_set_pins_with_mask(pio, sm, pin_values, pin_mask); return *this; }
	const StateMachine& set_pindirs_with_mask(uint32_t pin_dirs, uint32_t pin_mask) const { ::pio_sm_set_pindirs_with_mask(pio, sm, pin_dirs, pin_mask); return *this; }
public:
	const StateMachine& claim() const { ::pio_sm_claim(pio, sm); return *this; }
	const StateMachine& unclaim() const { ::pio_sm_unclaim(pio, sm); return *this; }
	bool is_claimed() const { return ::pio_sm_is_claimed(pio, sm); }
};

}

extern PIO::Block PIO0;
extern PIO::Block PIO1;
#if PICO_PIO_VERSION > 0
extern PIO::Block PIO2;
#endif

}

#endif
