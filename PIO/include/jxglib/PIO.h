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

#define PIOVAR_program(progName)				progName##_program
#define PIOVAR_get_default_config(progName)		progName##_program_get_default_config
#define PIOVAR_wrap_target(progName) 			progName##_wrap_target
#define PIOVAR_wrap(progName)					progName##_wrap
#define PIOVAR_pio_version(progName)			progName##_pio_version
#define PIOVAR_offset(progName, label)			progName##_offset_##label

namespace jxglib {

namespace PIO {

class Program;
class StateMachine;

//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------
template<typename... Pins> void CheckAdjacent(uint pin1, Pins... pins) {
	if constexpr (sizeof...(pins) > 0) {
		auto check_sequence = [](uint first, auto... rest) {
			uint expected = first + 1;
			return ((rest == expected++) && ...);
		};
		if (!check_sequence(pin1, pins...)) ::panic("pins must be adjacent\n");
	}
}

inline uint MakePinMask(uint base, uint count) { return ((1u << count) - 1) << base; }

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
	void Configure(const Program& program, uint offset);
public:
	Config& set_out_pin_base(uint out_base) { ::sm_config_set_out_pin_base(&c_, out_base); return *this; }
	Config& set_out_pin_count(uint out_count) { ::sm_config_set_out_pin_count(&c_, out_count); return *this; }
	Config& set_out_pins(uint out_base, uint out_count) { ::sm_config_set_out_pins(&c_, out_base, out_count); return *this; }
public:
	Config& set_set_pin_base(uint set_base) { ::sm_config_set_set_pin_base(&c_, set_base); return *this; }
	Config& set_set_pin_count(uint set_count) { ::sm_config_set_set_pin_count(&c_, set_count); return *this; }
	Config& set_set_pins(uint set_base, uint set_count) { ::sm_config_set_set_pins(&c_, set_base, set_count); return *this; }
public:
	Config& set_in_pin_base(uint in_base) { ::sm_config_set_in_pin_base(&c_, in_base); return *this; }
	Config& set_in_pins(uint in_base) { ::sm_config_set_in_pins(&c_, in_base); return *this; }
public:
	Config& set_sideset_pin_base(uint sideset_base) { ::sm_config_set_sideset_pin_base(&c_, sideset_base); return *this; }
	Config& set_sideset_pins(uint sideset_base) { ::sm_config_set_sideset_pins(&c_, sideset_base); return *this; }
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
		Variable(const char* label, uint16_t relAddr) : label_{label}, value_{relAddr} {};
		const char* GetLabel() const { return label_; }
		uint16_t GetValue() const { return value_; }
		void SetNext(Variable* pVariableNext) { pVariableNext_.reset(pVariableNext); }
		Variable* GetNext() const { return pVariableNext_.get(); }
	};
	struct Wrap {
		uint16_t relAddr_target;
		uint16_t relAddr_wrapPlus;
	};
	struct SideSet {
		uint bit_count;
		bool optional;
		bool pindirs;
	};
	enum class Directive {
		None,
		program,
		entry,
		origin,
		pio_version,
		side_set,
		wrap_target,
		wrap,
		end,
	};
	static const uint16_t RelAddrInvalid = 0xffff;
private:
	const char* name_;
	uint16_t instTbl_[PIO_INSTRUCTION_COUNT];
	uint16_t relAddrCur_;
	uint16_t relAddrEntry_;
	Wrap wrap_;
	SideSet sideSet_;
	pio_program_t program_;
private:
	Directive directive_;
	bool sideSpecifiedFlag_;
	std::unique_ptr<Variable> pVariableHead_;
	std::unique_ptr<Variable> pVariableRefHead_;
public:
	static const Program None;
public:
	Program();
public:
	void Reset();
	uint GetRelAddrCur() const { return relAddrCur_; }
	Program& SetRelAddrEntry(uint16_t relAddrEntry) { relAddrEntry_ = relAddrEntry; return *this; }
	uint GetRelAddrEntry() const { return relAddrEntry_; }
	const char* GetName() const { return name_; }
	const Wrap& GetWrap() const { return wrap_; }
	const SideSet& GetSideSet() const { return sideSet_; }
	const pio_program_t& GetEntity() const { return program_; }
	const pio_program_t* GetEntityPtr() const { return &program_; }
	Program& AddInst(uint16_t inst);
	Program& L(const char* label);
	Program& pub(uint* pRelAddr) { *pRelAddr = relAddrCur_; return *this; }
public:
	bool IsSideMust() const { return sideSet_.bit_count > 0 && !sideSet_.optional; }
	void AddVariable(const char* label, uint16_t relAddr);
	void AddVariableRef(const char* label, uint16_t relAddr);
	const Variable* LookupVariable(const char* label) const;
	Program& Complete();
public:
	operator const pio_program_t&() const { return program_; }
	//operator const pio_program_t*() const { return &program_; }	// conflicts with [] operator
public:
	const Program& exec(StateMachine& sm) const;
public:
	// Directives
	Program& program(const char* name)	{ Reset(); directive_ = Directive::program; name_ = name; return *this; }
	Program& entry()					{ directive_ = Directive::entry; relAddrEntry_ = relAddrCur_; return *this; }
	Program& origin(uint offset)		{ directive_ = Directive::origin; program_.origin = offset; return *this; }
	Program& pio_version(uint version)	{ directive_ = Directive::pio_version; program_.pio_version = version; return *this; }
	Program& side_set(int bit_count);
	Program& wrap_target(uint* pRelAddr = nullptr);
	Program& wrap(uint* pRelAddr = nullptr);
	Program& end(uint* pRelAddr = nullptr);
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
	Program& in(enum pio_src_dest src, uint16_t count) { return AddInst(::pio_encode_in(src, count & 0x1f)); }
	Program& out(enum pio_src_dest dest, uint16_t count) { return AddInst(::pio_encode_out(dest, count & 0x1f)); }
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
		return _pio_encode_instr_and_args(pio_instr_bits_wait, 3u | (polarity? 4u : 0u), pin_offset);
	}
public:
	// Method-Chain Assembler instructions
	Program& jmp(const char* label) { AddVariableRef(label, relAddrCur_); return jmp(static_cast<uint16_t>(0)); }
	Program& jmp(const char* cond, uint16_t addr);
	Program& jmp(const char* cond, const char* label) { AddVariableRef(label, relAddrCur_); return jmp(cond, static_cast<uint16_t>(0)); }
	Program& wait(bool polarity, const char* src, uint16_t index);
	Program& in(const char* src, uint16_t count) { return in(StrToSrc(pio_instr_bits_in, src), count); }
	Program& out(const char* dest, uint16_t count) { return out(StrToDest(pio_instr_bits_out, dest), count); }
	Program& push() { return push(false, true); }
	Program& pull() { return pull(false, true); }
	Program& mov(const char* dest, const char* src, uint16_t index = 0);
	Program& mov(const char* dest, uint16_t index, const char* src) { return mov(dest, src, index); }
	Program& irq(const char* op, uint16_t irq_n);
	Program& irq(uint16_t irq_n) { return irq("set", irq_n); }
	Program& set(const char* dest, uint16_t value) { return set(StrToDest(pio_instr_bits_set, dest), value); }
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
	pio_src_dest StrToSrc(uint16_t inst, const char* str) const;
	pio_src_dest StrToDest(uint16_t inst, const char* str) const;
	pio_src_dest StrToSrcDest(uint16_t inst, const char* str) const;
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
public:
	pio_hw_t* pio;
public:
	Block(pio_hw_t* pio = nullptr) : pio{pio} {}
	Block(uint iPIO) : Block{::pio_get_instance(iPIO)} {}
	Block(const Block& block) : pio{block.pio} {}
public:
	void Invalidate() { pio = nullptr; }
	bool IsValid() const { return !!pio; }
public:
	operator pio_hw_t*() const { return pio; }
public:
	const Block& gpio_init(uint pin) const { ::pio_gpio_init(pio, pin); return *this; }
	uint get_gpio_base() const { return ::pio_get_gpio_base(pio); }
	uint get_index() const { return ::pio_get_index(pio); }
	uint get_funcsel() const { return ::pio_get_funcsel(pio); }
	int set_gpio_base(uint gpio_base) const { return ::pio_set_gpio_base(pio, gpio_base); }
public:
	bool can_add_program(const pio_program_t* program) const { return ::pio_can_add_program(pio, program); }
	bool can_add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_can_add_program_at_offset(pio, program, offset); }
	int add_program(const pio_program_t* program) const { return ::pio_add_program(pio, program); }
	int add_program_at_offset(const pio_program_t* program, uint offset) const { return ::pio_add_program_at_offset(pio, program, offset); }
	const Block& remove_program(const pio_program_t* program, uint loaded_offset) const { ::pio_remove_program(pio, program, loaded_offset); return *this; }
	const Block& clear_instruction_memory() const { ::pio_clear_instruction_memory(pio); return *this; }
public:
	const Block& set_sm_mask_enabled(uint32_t mask, bool enabled) const { ::pio_set_sm_mask_enabled(pio, mask, enabled); return *this; }
	const Block& restart_sm_mask(uint32_t mask) const { ::pio_restart_sm_mask(pio, mask); return *this; }
	const Block& clkdiv_restart_sm_mask(uint32_t mask) const { ::pio_clkdiv_restart_sm_mask(pio, mask); return *this; }
	const Block& enable_sm_mask_in_sync(uint32_t mask) const { ::pio_enable_sm_mask_in_sync(pio, mask); return *this; }
public:
	const Block& set_irq0_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq0_source_enabled(pio, source, enabled); return *this; }
	const Block& set_irq1_source_enabled(pio_interrupt_source_t source, bool enabled) const { ::pio_set_irq1_source_enabled(pio, source, enabled); return *this; }
	const Block& set_irq0_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq0_source_mask_enabled(pio, source_mask, enabled); return *this; }
	const Block& set_irq1_source_mask_enabled(uint32_t source_mask, bool enabled) const { ::pio_set_irq1_source_mask_enabled(pio, source_mask, enabled); return *this; }
	const Block& set_irqn_source_enabled(uint irq_index, pio_interrupt_source_t source, bool enabled) const { ::pio_set_irqn_source_enabled(pio, irq_index, source, enabled); return *this; }
	const Block& set_irqn_source_mask_enabled(uint irq_index, uint32_t source_mask, bool enabled) const { ::pio_set_irqn_source_mask_enabled(pio, irq_index, source_mask, enabled); return *this; }
	bool interrupt_get(uint pio_interrupt_num) const { return ::pio_interrupt_get(pio, pio_interrupt_num); }
	const Block& interrupt_clear(uint pio_interrupt_num) const { ::pio_interrupt_clear(pio, pio_interrupt_num); return *this; }
	int get_irq_num(uint irqn) const { return ::pio_get_irq_num(pio, irqn); }
public:
	const Block& claim_sm_mask(uint sm_mask) const { ::pio_claim_sm_mask(pio, sm_mask); return *this; }
	int claim_unused_sm(bool required) const { return ::pio_claim_unused_sm(pio, required); }
public:
	bool is_vacant() const { return !(::pio_sm_is_claimed(pio, 0) || ::pio_sm_is_claimed(pio, 1) || ::pio_sm_is_claimed(pio, 2) || ::pio_sm_is_claimed(pio, 3)); }
public:	
	static pio_hw_t* get_instance(uint instance) { return ::pio_get_instance(instance); }
};

//------------------------------------------------------------------------------
// PIO::StateMachine
//------------------------------------------------------------------------------
class StateMachine {
public:
	Block pio;
	uint sm;
	uint offset;
	Config config;
private:
	uint32_t pin_dirs_;
	uint32_t pin_mask_;
	const Program* pProgram_;
	StateMachine* pSmToShareProgram_;
public:
	StateMachine() : sm{static_cast<uint>(-1)}, offset{0}, pin_dirs_{0}, pin_mask_{0}, pProgram_{&Program::None}, pSmToShareProgram_{nullptr} {}
	StateMachine(const StateMachine& stateMachine) : pio{stateMachine.pio}, sm{stateMachine.sm},
		offset{stateMachine.offset}, config{stateMachine.config}, pin_dirs_{stateMachine.pin_dirs_}, pin_mask_{stateMachine.pin_mask_},
		pProgram_{stateMachine.pProgram_}, pSmToShareProgram_{stateMachine.pSmToShareProgram_} {}
	~StateMachine();
public:
	operator uint() { return sm; }
public:
	bool IsValid() const { return !!pio && sm != static_cast<uint>(-1); }
	void Invalidate() { pio = nullptr, sm = static_cast<uint>(-1), offset = 0; }
	const Program& GetProgram() const { return *pProgram_; }
public:
	StateMachine& set_program(const Program& program);
	StateMachine& set_program(const Program& program, pio_hw_t* pio);
	StateMachine& set_program(const Program& program, pio_hw_t* pio, uint sm);
	StateMachine& share_program(StateMachine& smToShareProgram);
	StateMachine& share_program(StateMachine& smToShareProgram, pio_hw_t* pio);
	StateMachine& share_program(StateMachine& smToShareProgram, pio_hw_t* pio, uint sm);
	//StateMachine& claim_resource(uint gpio_base, uint gpio_count, bool set_gpio_base);
	StateMachine& remove_program();
private:
	void setup_resource_(pio_hw_t* pio, uint sm, uint offset);
public:
	StateMachine& reserve_out_pins(uint base, uint count);
	template<typename... Pins> StateMachine& reserve_out_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return reserve_out_pins(pin1, 1 + sizeof...(pins));
	}
	StateMachine& reserve_set_pins(uint base, uint count);
	template<typename... Pins> StateMachine& reserve_set_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return reserve_set_pins(pin1, 1 + sizeof...(pins));
	}
	StateMachine& reserve_in_pins(uint base, uint count);
	template<typename... Pins> StateMachine& reserve_in_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return reserve_in_pins(pin1, 1 + sizeof...(pins));
	}
	StateMachine& reserve_listen_pins(uint base, uint count); // count is not used, just for compatibility with reserve_in_pins()
	template<typename... Pins> StateMachine& reserve_listen_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return reserve_listen_pins(pin1, -1);
	}
	StateMachine& reserve_sideset_pins(uint base, uint count);
	template<typename... Pins> StateMachine& reserve_sideset_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return reserve_sideset_pins(pin1, 1 + sizeof...(pins));
	}
public:
	StateMachine& claim() { ::pio_sm_claim(pio, sm); return *this; }
	StateMachine& unclaim() { ::pio_sm_unclaim(pio, sm); return *this; }
	bool is_claimed() { return ::pio_sm_is_claimed(pio, sm); }
public:
	StateMachine& gpio_init(uint pin) { pio.gpio_init(pin); return *this; }
public:
	StateMachine& set_in_pins(uint in_base) { ::pio_sm_set_in_pins(pio, sm, in_base); return *this; }
	template<typename... Pins>
	StateMachine& set_in_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return set_in_pins(pin1);
	}
public:
	StateMachine& set_out_pins(uint out_base, uint out_count) { ::pio_sm_set_out_pins(pio, sm, out_base, out_count); return *this; }
	template<typename... Pins>
	StateMachine& set_out_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return set_out_pins(pin1, 1 + sizeof...(pins));
	}
public:
	StateMachine& set_set_pins(uint set_base, uint set_count) { ::pio_sm_set_set_pins(pio, sm, set_base, set_count); return *this; }
	template<typename... Pins>
	StateMachine& set_set_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return set_set_pins(pin1, 1 + sizeof...(pins));
	}
public:
	StateMachine& set_sideset_pins(uint sideset_base) { ::pio_sm_set_sideset_pins(pio, sm, sideset_base); return *this; }
	template<typename... Pins>
	StateMachine& set_sideset_pin(uint pin1, Pins... pins) {
		if constexpr (sizeof...(pins) > 0) CheckAdjacent(pin1, pins...);
		return set_sideset_pins(pin1);
	}
public:
	StateMachine& set_jmp_pin(uint pin) { ::pio_sm_set_jmp_pin(pio, sm, pin); return *this; }
public:
	StateMachine& set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) { ::pio_sm_set_clkdiv_int_frac(pio, sm, div_int, div_frac); return *this; }
	StateMachine& set_clkdiv(float div) { ::pio_sm_set_clkdiv(pio, sm, div); return *this; }
	StateMachine& set_wrap(uint wrap_target, uint wrap) { ::pio_sm_set_wrap(pio, sm, wrap_target, wrap); return *this; }
public:
	StateMachine& init(uint initial_pc, const pio_sm_config* config);
	StateMachine& init_with_entry(uint relAddrEntry) { return init(offset + relAddrEntry, config); }
	StateMachine& init() { return init_with_entry(GetProgram().GetRelAddrEntry()); }
	StateMachine& init_pins();
	int set_config(const pio_sm_config *config) { return ::pio_sm_set_config(pio, sm, config); }
	StateMachine& set_enabled(bool enabled = true) { ::pio_sm_set_enabled(pio, sm, enabled); return *this; }
	StateMachine& restart() { ::pio_sm_restart(pio, sm); return *this; }
	StateMachine& clkdiv_restart() { ::pio_sm_clkdiv_restart(pio, sm); return *this; }
	StateMachine& exec(uint instr) { ::pio_sm_exec(pio, sm, instr); return *this; }
	StateMachine& exec(const Program& program);
	bool is_exec_stalled() { return ::pio_sm_is_exec_stalled(pio, sm); }
	StateMachine& exec_wait_blocking(uint instr) { ::pio_sm_exec_wait_blocking(pio, sm, instr); return *this; }
public:
	StateMachine& set_consecutive_pindirs(uint pins_base, uint pin_count, bool is_out) { ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, is_out); return *this; }
	StateMachine& set_consecutive_pindirs_out(uint pins_base, uint pin_count) { ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, true); return *this; }
	StateMachine& set_consecutive_pindirs_in(uint pins_base, uint pin_count) { ::pio_sm_set_consecutive_pindirs(pio, sm, pins_base, pin_count, false); return *this; }
public:
	StateMachine& set_pindirs_with_mask(uint32_t pin_dirs, uint32_t pin_mask) { ::pio_sm_set_pindirs_with_mask(pio, sm, pin_dirs, pin_mask); return *this; }
	template<typename... Pins> StateMachine& set_pindir_out(uint pin1, Pins... pins) {
		uint32_t pin_mask = (1u << pin1);
		((pin_mask |= (1u << pins)), ...);
		set_pindirs_with_mask(pin_mask, pin_mask);  // Set directions to output (1)
		return *this;
	}
	template<typename... Pins> StateMachine& set_pindir_in(uint pin1, Pins... pins) {
		uint32_t pin_mask = (1u << pin1);
		((pin_mask |= (1u << pins)), ...);
		set_pindirs_with_mask(0, pin_mask);  // Set directions to input (0)
		return *this;
	}
	StateMachine& set_pindir_out_mask(uint32_t pin_mask) { set_pindirs_with_mask(pin_mask, pin_mask); return *this; }
	StateMachine& set_pindir_in_mask(uint32_t pin_mask) { set_pindirs_with_mask(0, pin_mask); return *this; }
public:
	StateMachine& set_pins(uint32_t pin_values) { ::pio_sm_set_pins(pio, sm, pin_values); return *this; }
	StateMachine& set_pins_with_mask(uint32_t pin_values, uint32_t pin_mask) { ::pio_sm_set_pins_with_mask(pio, sm, pin_values, pin_mask); return *this; }
	template<typename... Pins> int set_pin(bool value, uint pin1, Pins... pins) {
		uint32_t pin_mask = (1u << pin1);
		((pin_mask |= (1u << pins)), ...);
		set_pins_with_mask(value? pin_mask : 0, pin_mask);
		return 0;
	}
public:
	uint get_dreq(bool is_tx) { return ::pio_get_dreq(pio, sm, is_tx); }
	uint get_dreq_tx() { return ::pio_get_dreq(pio, sm, true); }
	uint get_dreq_rx() { return ::pio_get_dreq(pio, sm, false); }
	const volatile uint32_t* get_txf() { return reinterpret_cast<const volatile uint32_t*>(&pio.pio->txf[sm]); }
	const volatile uint32_t* get_rxf() { return reinterpret_cast<const volatile uint32_t*>(&pio.pio->rxf[sm]); }
public:
	uint8_t get_pc() { return  ::pio_sm_get_pc(pio, sm); }
public:
	bool is_rx_fifo_full() { return ::pio_sm_is_rx_fifo_full(pio, sm); }
	bool is_tx_fifo_full() { return ::pio_sm_is_tx_fifo_full(pio, sm); }
	bool is_rx_fifo_empty() { return ::pio_sm_is_rx_fifo_empty(pio, sm); }
	bool is_tx_fifo_empty() { return ::pio_sm_is_tx_fifo_empty(pio, sm); }
	uint get_rx_fifo_level() { return ::pio_sm_get_rx_fifo_level(pio, sm); }
	uint get_tx_fifo_level() { return ::pio_sm_get_tx_fifo_level(pio, sm); }
	pio_interrupt_source_t get_rx_fifo_not_empty_interrupt_source() { return ::pio_get_rx_fifo_not_empty_interrupt_source(sm); }
	pio_interrupt_source_t get_tx_fifo_not_full_interrupt_source() { return ::pio_get_tx_fifo_not_full_interrupt_source(sm); }
	StateMachine& drain_tx_fifo() { ::pio_sm_drain_tx_fifo(pio, sm); return *this; }
	StateMachine& clear_fifos() { ::pio_sm_clear_fifos(pio, sm); return *this; }
public:
	StateMachine& put(uint32_t data) { ::pio_sm_put(pio, sm, data); return *this; }
	uint32_t get() { return ::pio_sm_get(pio, sm); }
	StateMachine& put_blocking(uint32_t data) { ::pio_sm_put_blocking(pio, sm, data); return *this; }
	uint32_t get_blocking() { return ::pio_sm_get_blocking(pio, sm); }
public:
	StateMachine& config_set_out_pin_base(uint out_base) { config.set_out_pin_base(out_base); return *this; }
	StateMachine& config_set_out_pin_count(uint out_count) { config.set_out_pin_count(out_count); return *this; }
	StateMachine& config_set_out_pins(uint out_base, uint out_count) { config.set_out_pins(out_base, out_count); return *this; }
public:
	StateMachine& config_set_set_pin_base(uint set_base) { config.set_set_pin_base(set_base); return *this; }
	StateMachine& config_set_set_pin_count(uint set_count) { config.set_set_pin_count(set_count); return *this; }
	StateMachine& config_set_set_pins(uint set_base, uint set_count) { config.set_set_pins(set_base, set_count); return *this; }
public:
	StateMachine& config_set_in_pin_base(uint in_base) { config.set_in_pin_base(in_base); return *this; }
	StateMachine& config_set_in_pins(uint in_base) { config.set_in_pins(in_base); return *this; }
public:
	StateMachine& config_set_sideset_pin_base(uint sideset_base) { config.set_sideset_pin_base(sideset_base); return *this; }
	StateMachine& config_set_sideset_pins(uint sideset_base) { config.set_sideset_pins(sideset_base); return *this; }
public:
	StateMachine& config_set_jmp_pin(uint pin) { config.set_jmp_pin(pin); return *this; }
public:
	StateMachine& config_set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac) { config.set_clkdiv_int_frac(div_int, div_frac); return *this; }
	StateMachine& config_set_clkdiv(float div) { config.set_clkdiv(div); return *this; }
public:
	StateMachine& config_set_sideset(uint bit_count, bool optional, bool pindirs) { config.set_sideset(bit_count, optional, pindirs); return *this; }
	StateMachine& config_set_wrap(uint wrap_target, uint wrap) { config.set_wrap(wrap_target, wrap); return *this; }
public:
	StateMachine& config_set_in_shift(bool shift_right, bool autopush = false, uint push_threshold = 32) { config.set_in_shift(shift_right, autopush, push_threshold); return *this; }
	StateMachine& config_set_in_shift_left(bool autopush = false, uint push_threshold = 32) { return config_set_in_shift(false, autopush, push_threshold); }
	StateMachine& config_set_in_shift_right(bool autopush = false, uint push_threshold = 32) { return config_set_in_shift(true, autopush, push_threshold); }
public:
	StateMachine& config_set_out_shift(bool shift_right, bool autopull = false, uint pull_threshold = 32) { config.set_out_shift(shift_right, autopull, pull_threshold); return *this; }
	StateMachine& config_set_out_shift_left(bool autopull = false, uint pull_threshold = 32) { return config_set_out_shift(false, autopull, pull_threshold); }
	StateMachine& config_set_out_shift_right(bool autopull = false, uint pull_threshold = 32) { return config_set_out_shift(true, autopull, pull_threshold); }
public:
	StateMachine& config_set_fifo_join(enum pio_fifo_join join) { config.set_fifo_join(join); return *this; }
	StateMachine& config_set_fifo_join_none() { return config_set_fifo_join(PIO_FIFO_JOIN_NONE); }
	StateMachine& config_set_fifo_join_tx() { return config_set_fifo_join(PIO_FIFO_JOIN_TX); }
	StateMachine& config_set_fifo_join_rx() { return config_set_fifo_join(PIO_FIFO_JOIN_RX); }
#if PICO_PIO_VERSION > 0
	StateMachine& config_set_fifo_join_txput() { return config_set_fifo_join(PIO_FIFO_JOIN_TXPUT); }
	StateMachine& config_set_fifo_join_txget() { return config_set_fifo_join(PIO_FIFO_JOIN_TXGET); }
	StateMachine& config_set_fifo_join_putget() { return config_set_fifo_join(PIO_FIFO_JOIN_PUTGET); }
#endif
public:
	StateMachine& config_set_out_special(bool sticky, bool has_enable_pin, uint enable_pin_index) { config.set_out_special(sticky, has_enable_pin, enable_pin_index); return *this; }
	StateMachine& config_set_mov_status(enum pio_mov_status_type status_sel, uint status_n) { config.set_mov_status(status_sel, status_n); return *this; }
};

#if PICO_PIO_VERSION > 0
constexpr uint Num = 3;
#else
constexpr uint Num = 2;
#endif

}

extern PIO::Block PIO0;
extern PIO::Block PIO1;
#if PICO_PIO_VERSION > 0
extern PIO::Block PIO2;
#endif

}

#endif
