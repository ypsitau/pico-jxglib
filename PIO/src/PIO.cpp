//==============================================================================
// PIO.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/PIO.h"
#include "hardware/clocks.h"

namespace jxglib {

PIO::Block PIO0(pio0);
PIO::Block PIO1(pio1);
#if PICO_PIO_VERSION > 0
PIO::Block PIO2(pio2);
#endif

namespace PIO {

//------------------------------------------------------------------------------
// PIO::Config
//------------------------------------------------------------------------------
void Config::Configure(const Program& program, uint offset)
{
	const Program::Wrap& wrap = program.GetWrap();
	const Program::SideSet& sideSet = program.GetSideSet();
	::sm_config_set_sideset(&c_, sideSet.bit_count, sideSet.optional, sideSet.pindirs);
	if (wrap.relAddr_wrapPlus > 0) {
		::sm_config_set_wrap(&c_, offset + wrap.relAddr_target, offset + wrap.relAddr_wrapPlus - 1);
	}
}

//------------------------------------------------------------------------------
// PIO::StateMachine
//------------------------------------------------------------------------------
StateMachine::~StateMachine()
{
	if (IsValid()) remove_program();
}

StateMachine& StateMachine::set_program(const Program& program)
{
	pio_hw_t* pioClaimed;
	uint smClaimed;
	uint offsetClaimed;
	if (!::pio_claim_free_sm_and_add_program(program.GetEntityPtr(), &pioClaimed, &smClaimed, &offsetClaimed)) {
		::panic("failed to claim free state machine and add program");
	}
	pProgram_ = &program;
	pSmToShareProgram_ = nullptr;
	setup_resource_(pioClaimed, smClaimed, offsetClaimed);
	return *this;
}

StateMachine& StateMachine::set_program(const Program& program, pio_hw_t* pio)
{
	uint offsetClaimed = ::pio_add_program(pio, program.GetEntityPtr());
	pProgram_ = &program;
	pSmToShareProgram_ = nullptr;
	setup_resource_(pio, ::pio_claim_unused_sm(pio, true), offsetClaimed);
	return *this;
}

StateMachine& StateMachine::set_program(const Program& program, pio_hw_t* pio, uint sm)
{
	uint offsetClaimed = ::pio_add_program(pio, program.GetEntityPtr());
	pProgram_ = &program;
	pSmToShareProgram_ = nullptr;
	::pio_sm_claim(pio, sm);
	setup_resource_(pio, sm, offsetClaimed);
	return *this;
}

StateMachine& StateMachine::share_program(StateMachine& smToShareProgram)
{
	pProgram_ = smToShareProgram.pProgram_;
	pSmToShareProgram_ = &smToShareProgram;
	setup_resource_(smToShareProgram.pio, ::pio_claim_unused_sm(smToShareProgram.pio, true), smToShareProgram.offset);
	return *this;
	//return share_program(smToShareProgram, smToShareProgram.pio, ::pio_claim_unused_sm(smToShareProgram.pio, true));
}

StateMachine& StateMachine::share_program(StateMachine& smToShareProgram, pio_hw_t* pio)
{
	pProgram_ = smToShareProgram.pProgram_;
	pSmToShareProgram_ = &smToShareProgram;
	setup_resource_(pio, ::pio_claim_unused_sm(pio, true), smToShareProgram.offset);
	return *this;
}

StateMachine& StateMachine::share_program(StateMachine& smToShareProgram, pio_hw_t* pio, uint sm)
{
	pProgram_ = smToShareProgram.pProgram_;
	pSmToShareProgram_ = &smToShareProgram;
	::pio_sm_claim(pio, sm);
	setup_resource_(pio, sm, smToShareProgram.offset);
	return *this;
}

#if 0
StateMachine& StateMachine::claim_resource(uint gpio_base, uint gpio_count, bool set_gpio_base)
{
	pio_hw_t* pioClaimed;
	if (!::pio_claim_free_sm_and_add_program_for_gpio_range(GetProgram().GetEntityPtr(), &pioClaimed, &sm, &offset, gpio_base, gpio_count, set_gpio_base)) {
		::panic("failed to claim free state machine and add program for GPIO range");
	}
	pio = pioClaimed;
	config.Configure(GetProgram(), offset);
	return *this;
}
#endif

StateMachine& StateMachine::remove_program()
{
	if (pSmToShareProgram_) {
		::pio_sm_unclaim(pio, sm);
	} else {
		::pio_remove_program_and_unclaim_sm(GetProgram().GetEntityPtr(), pio, sm, offset);
	}
	Invalidate();
	return *this;
}

void StateMachine::setup_resource_(pio_hw_t* pio, uint sm, uint offset)
{
	this->pio = pio;
	this->sm = sm;
	this->offset = offset;
	config.Configure(GetProgram(), offset);
	init_pins();
	init();
}

const StateMachine& StateMachine::init(uint initial_pc, const pio_sm_config* config) const
{
	init_pins();
	if (::pio_sm_init(pio, sm, initial_pc, config) < 0) ::panic("failed to initialize PIO's state machine");
	return *this;
}

const StateMachine& StateMachine::init_pins() const
{
	uint32_t pinBits = pin_mask_;
	for (uint pin = 0; pinBits; pinBits >>= 1, ++pin) if (pinBits & 1) ::pio_gpio_init(pio, pin);
	set_pindirs_with_mask(pin_dirs_, pin_mask_);
	return *this;
}

StateMachine& StateMachine::reserve_out_pins(uint base, uint count)
{
	uint32_t pinMask = MakePinMask(base, count);
	pin_mask_ |= pinMask;
	pin_dirs_ |= pinMask;
	config.set_out_pins(base, count);
	return *this;
}

StateMachine& StateMachine::reserve_set_pins(uint base, uint count)
{
	uint32_t pinMask = MakePinMask(base, count);
	pin_mask_ |= pinMask;
	pin_dirs_ |= pinMask;
	config.set_set_pins(base, count);
	return *this;
}

StateMachine& StateMachine::reserve_in_pins(uint base, uint count)
{
	uint32_t pinMask = MakePinMask(base, count);
	pin_mask_ |= pinMask;
	pin_dirs_ &= ~pinMask;
	config.set_in_pins(base);
	return *this;
}

StateMachine& StateMachine::reserve_listen_pins(uint base, uint count)
{
	config.set_in_pins(base);
	return *this;
}

StateMachine& StateMachine::reserve_sideset_pins(uint base, uint count)
{
	uint32_t pinMask = MakePinMask(base, count);
	pin_mask_ |= pinMask;
	pin_dirs_ |= pinMask;
	config.set_sideset_pins(base);
	return *this;
}

const StateMachine& StateMachine::exec(const Program& program) const
{
	program.exec(*this);
	return *this;
}

//------------------------------------------------------------------------------
// PIO::Block
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PIO::Program
//------------------------------------------------------------------------------
const Program Program::None;

Program::Program() : name_{""}, relAddrCur_{0}, relAddrEntry_{0}, wrap_{0, 0}, sideSet_{0, false, false}, directive_{Directive::None}, sideSpecifiedFlag_{false}
{
	for (int i = 0; i < count_of(instTbl_); ++i) instTbl_[i] = 0x0000;
	program_.instructions = instTbl_;
	program_.length = 0;
	program_.origin = -1; // required instruction memory origin or -1
	program_.pio_version = 0;
#if PICO_PIO_VERSION > 0
	program_.used_gpio_ranges = 0; // bitmap with one bit per 16 pins
#endif
}

void Program::Reset()
{
	name_ = "";
	relAddrCur_ = 0;
	wrap_ = {0, 0};
	sideSet_ = {0, false, false};
	for (int i = 0; i < count_of(instTbl_); ++i) instTbl_[i] = 0x0000;
	program_.instructions = instTbl_;
	program_.length = 0;
	program_.origin = -1; // required instruction memory origin or -1
	program_.pio_version = 0;
#if PICO_PIO_VERSION > 0
	program_.used_gpio_ranges = 0; // bitmap with one bit per 16 pins
#endif
	directive_ = Directive::None;
	sideSpecifiedFlag_ = false;
	pVariableHead_.reset();
	pVariableRefHead_.reset();
}

Program& Program::AddInst(uint16_t inst)
{
	directive_ = Directive::None; // reset directive
	if (relAddrCur_ > 0 && IsSideMust() && !sideSpecifiedFlag_) {
		::panic("addr%02x: side-set must be specified for each instruction\n", relAddrCur_);
	}
	if (relAddrCur_ >= count_of(instTbl_)) ::panic("addr%02x: too many PIO instructions\n", relAddrCur_);
	instTbl_[relAddrCur_++] = inst;
	sideSpecifiedFlag_ = false;
	return *this;
}

Program& Program::L(const char* label)
{
	if (LookupVariable(label)) ::panic("addr%02x: label '%s' already defined\n", relAddrCur_, label);
	AddVariable(label, relAddrCur_);
	return *this;
}

void Program::AddVariable(const char* label, uint16_t relAddr)
{
	Variable* pVariable = new Variable(label, relAddr);
	pVariable->SetNext(pVariableHead_.release());
	pVariableHead_.reset(pVariable);
}

void Program::AddVariableRef(const char* label, uint16_t relAddr)
{
	Variable* pVariable = new Variable(label, relAddr);
	pVariable->SetNext(pVariableRefHead_.release());
	pVariableRefHead_.reset(pVariable);
}

const Program::Variable* Program::LookupVariable(const char* label) const
{
	for (const Variable* pVariable = pVariableHead_.get(); pVariable; pVariable = pVariable->GetNext()) {
		if (::strcmp(pVariable->GetLabel(), label) == 0) return pVariable;
	}
	return nullptr;
}

Program& Program::Complete()
{
	for (Variable* pVariableRef = pVariableRefHead_.get(); pVariableRef; pVariableRef = pVariableRef->GetNext()) {
		const Variable* pVariable = LookupVariable(pVariableRef->GetLabel());
		if (pVariable) {
			uint16_t& inst = instTbl_[pVariableRef->GetValue()];
			inst = inst & 0xffe0 | pVariable->GetValue();
		} else {
			::panic("addr%02x: label '%s' not found\n", relAddrCur_, pVariableRef->GetLabel());
		}
	}
	program_.length = relAddrCur_;
	if (wrap_.relAddr_wrapPlus == 0) wrap_.relAddr_wrapPlus = relAddrCur_;
	pVariableHead_.reset();
	pVariableRefHead_.reset();
	return *this;
}

Program& Program::side_set(int bit_count)
{
	if (bit_count < 0 || bit_count > 5) ::panic("addr%02x: side_set(): bit count out of range\n", relAddrCur_);
	directive_ = Directive::side_set;
	sideSet_.bit_count = bit_count;
	return *this;
}

Program& Program::wrap_target(uint* pRelAddr)
{
	directive_ = Directive::wrap_target;
	wrap_.relAddr_target = relAddrCur_;
	if (pRelAddr) *pRelAddr = relAddrCur_;
	return *this;
}

Program& Program::wrap(uint* pRelAddr)
{
	directive_ = Directive::wrap;
	wrap_.relAddr_wrapPlus = relAddrCur_;
	if (pRelAddr) *pRelAddr = relAddrCur_;
	return *this;
}

Program& Program::end(uint* pRelAddr)
{
	directive_ = Directive::end;
	if (relAddrCur_ > 0 && IsSideMust() && !sideSpecifiedFlag_) {
		::panic("addr%02x: side-set must be specified for each instruction\n", relAddrCur_);
	}
	if (pRelAddr) *pRelAddr = relAddrCur_;
	return Complete();
}

Program& Program::jmp(const char* cond, uint16_t addr)
{
	if (::strcasecmp(cond, "!x") == 0) return jmp_not_x(addr);
	if (::strcasecmp(cond, "x--") == 0) return jmp_x_dec(addr);
	if (::strcasecmp(cond, "!y") == 0) return jmp_not_y(addr);
	if (::strcasecmp(cond, "y--") == 0) return jmp_y_dec(addr);
	if (::strcasecmp(cond, "x!=y") == 0 || ::strcasecmp(cond, "x != y") == 0) return jmp_x_ne_y(addr);
	if (::strcasecmp(cond, "pin") == 0) return jmp_pin(addr);
	if (::strcasecmp(cond, "!osre") == 0) return jmp_not_osre(addr);
	::panic("addr%02x: jmp(): invalid condition '%s'\n", relAddrCur_, cond);
	return *this;
}

Program& Program::wait(bool polarity, const char* src, uint16_t index)
{
	if (::strcasecmp(src, "gpio") == 0)		return wait_gpio(polarity, index);
	if (::strcasecmp(src, "pin") == 0)		return wait_pin(polarity, index);
	if (::strcasecmp(src, "irq") == 0)		return wait_irq(polarity, false, index);
	if (::strcasecmp(src, "jmppin") == 0) {
		if (program_.pio_version < 1) ::panic("addr%02x: wait(): jmppin is not supported in PIO version %d\n", relAddrCur_, program_.pio_version);
		return wait_jmppin(polarity, index);
	}
	::panic("addr%02x: wait(): invalid source '%s'\n", relAddrCur_, src);
	return *this;
}

Program& Program::mov(const char* dest, const char* src, uint16_t index)
{
	if (StartsWithICase(dest, "rxfifo[")) {
		if (program_.pio_version < 1) ::panic("addr%02x: mov(): rxfifo[] is not supported in PIO version %d\n", relAddrCur_, program_.pio_version);
		dest += 7;
		if (::strcasecmp(src, "isr") != 0) ::panic("addr%02x: mov(): invalid source '%s'\n", relAddrCur_, src);
		uint16_t inst = 0;
		if (::strcasecmp(dest, "y]") == 0) {
			inst = 0b10000000'00010000;
		} else if (::strcasecmp(dest, "]") == 0) {
			inst = 0b10000000'00011000 | index;
		} else {
			char* endptr;
			int index = ::strtol(dest, &endptr, 0);
			if (::strcmp(endptr, "]") != 0) ::panic("addr%02x: mov(): invalid destination '%s'\n", relAddrCur_, dest);
			if (index < 0 || index > 7) ::panic("addr%02x: mov(): index out of range %d\n", relAddrCur_, index);
			inst = 0b10000000'00011000 | static_cast<uint16_t>(index);
		}
		return word(inst);
	} else if (StartsWithICase(src, "rxfifo[")) {
		if (program_.pio_version < 1) ::panic("addr%02x: mov(): rxfifo[] is not supported in PIO version %d\n", relAddrCur_, program_.pio_version);
		src += 7;
		if (::strcasecmp(dest, "osr") != 0) ::panic("addr%02x: mov(): invalid destination '%s'\n", relAddrCur_, dest);
		uint16_t inst = 0;
		if (::strcasecmp(src, "y]") == 0) {
			inst = 0b10000000'10010000;
		} else if (::strcasecmp(src, "]") == 0) {
			inst = 0b10000000'10011000 | index;
		} else {
			char* endptr;
			int index = ::strtol(src, &endptr, 0);
			if (::strcmp(endptr, "]") != 0) ::panic("addr%02x: mov(): invalid source '%s'\n", relAddrCur_, src);
			if (index < 0 || index > 7) ::panic("addr%02x: mov(): index out of range %d\n", relAddrCur_, index);
			inst = 0b10000000'10011000 | static_cast<uint16_t>(index);
		}
		return word(inst);
	} else {
		if (*src == '!' || *src == '~') return mov_not(StrToDest(pio_instr_bits_mov, dest), StrToSrc(pio_instr_bits_mov, src + 1));
		if (*src == ':' && *(src + 1) == ':') return mov_reverse(StrToDest(pio_instr_bits_mov, dest), StrToSrc(pio_instr_bits_mov, src + 2));
		return mov(StrToDest(pio_instr_bits_mov, dest), StrToSrc(pio_instr_bits_mov, src));
	}
}

Program& Program::irq(const char* op, uint16_t irq_n)
{
	bool relative = false;
	if (::strcasecmp(op, "set") == 0) return irq_set(irq_n, relative);
	if (::strcasecmp(op, "nowait") == 0) return irq_set(irq_n, relative);
	if (::strcasecmp(op, "wait") == 0) return irq_wait(irq_n, relative);
	if (::strcasecmp(op, "clear") == 0) return irq_clear(irq_n, relative);
	::panic("addr%02x: irq(): invalid operation '%s'\n", relAddrCur_, op);
	return *this;
}

Program& Program::opt()
{
	if (directive_ == Directive::side_set) {
		if (sideSet_.bit_count >= 5) ::panic("addr%02x: opt(): side-set bit count out of range\n", relAddrCur_);
		sideSet_.optional = true;
	} else {
		::panic("addr%02x: opt() is only applicable for side_set\n", relAddrCur_);
	}
	return *this;
}

Program& Program::pindirs()
{
	if (directive_ == Directive::side_set) {
		sideSet_.pindirs = true;
	} else {
		::panic("addr%02x: pindirs() is only applicable for side_set\n", relAddrCur_);
	}
	return *this;
}

Program& Program::side(uint16_t bits)
{
	if (sideSet_.bit_count == 0) ::panic("addr%02x: side(): side-set not defined\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (sideSet_.optional) {
		int lsb = 12 - sideSet_.bit_count;
		inst = inst | (1 << 12) | (bits << lsb);
	} else {
		int lsb = 13 - sideSet_.bit_count;
		inst = inst | (bits << lsb);
	}
	sideSpecifiedFlag_ = true;
	return *this;
}

Program& Program::delay(uint16_t cycles)
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	int nBits = 5 - sideSet_.bit_count;
	uint16_t cyclesMax = (1 << nBits) - 1;
	if (cycles > cyclesMax) ::panic("addr%02x: delay(): cycles out of range %d (max %d)\n", relAddrCur_, cycles, cyclesMax);
	inst = inst | (cycles << 8);
	return *this;
}

Program& Program::rel()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_IRQ(inst)) {
		inst |= (2 << 3);
	} else if (Is_WAIT(inst) && ((inst >> 5) & 0x3) == 2) {	// wait irq
		inst |= (2 << 3);
	} else {
		::panic("addr%02x: rel() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::prev()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_IRQ(inst)) {
		inst |= (1 << 3);
	} else if (Is_WAIT(inst) && ((inst >> 5) & 0x3) == 2) {	// wait irq
		inst |= (1 << 3);
	} else {
		::panic("addr%02x: prev() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::next()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_IRQ(inst)) {
		inst |= (3 << 3);
	} else if (Is_WAIT(inst) && ((inst >> 5) & 0x3) == 2) {	// wait irq
		inst |= (3 << 3);
	} else {
		::panic("addr%02x: next() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::iffull()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_PUSH(inst)) {
		inst |= (1 << 6);
	} else {
		::panic("addr%02x: iffull() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::ifempty()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_PULL(inst)) {
		inst |= (1 << 6);
	} else {
		::panic("addr%02x: iffull() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::block()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_PUSHorPULL(inst)) {
		inst |= (1 << 5);
	} else {
		::panic("addr%02x: block() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::noblock()
{
	if (relAddrCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", relAddrCur_);
	uint16_t& inst = instTbl_[relAddrCur_ - 1];
	if (Is_PUSHorPULL(inst)) {
		inst &= ~(1 << 5);
	} else {
		::panic("addr%02x: block() is not applicable for %s\n", relAddrCur_, GetInstName(inst));
	}
	return *this;
}

const Program& Program::exec(const StateMachine& sm) const
{
	for (uint relAddr = 0; relAddr < relAddrCur_; ++relAddr) {
		sm.exec(instTbl_[relAddr]);
		while (sm.is_exec_stalled()) Tickable::Tick();
	}
	return *this;
}

const char* Program::GetInstName(uint16_t inst)
{
	if (Is_JMP(inst))		return "JMP";
	if (Is_WAIT(inst))		return "WAIT";
	if (Is_IN(inst))		return "IN";
	if (Is_OUT(inst))		return "OUT";
	if (Is_PUSH(inst))		return "PUSH";
	if (Is_PULL(inst))		return "PULL";
	if (Is_MOVtoRX(inst))	return "MOV (to RX)";
	if (Is_MOVfromRX(inst))	return "MOV (from RX)";
	if (Is_MOV(inst))		return "MOV";
	if (Is_IRQ(inst))		return "IRQ";
	if (Is_SET(inst))		return "SET";
	return "unknown";
}

pio_src_dest Program::StrToSrc(uint16_t inst, const char* str) const
{
	pio_src_dest src = StrToSrcDest(inst, str);
	if (Is_IN(inst) && (src & _PIO_INVALID_IN_SRC) != 0) ::panic("addr%02x: invalid source '%s' for IN instruction\n", relAddrCur_, str);
	if (Is_MOV(inst) && (src & _PIO_INVALID_MOV_SRC) != 0) ::panic("addr%02x: invalid source '%s' for MOV instruction\n", relAddrCur_, str);
	return src;
}

pio_src_dest Program::StrToDest(uint16_t inst, const char* str) const
{
	pio_src_dest pio_pindirs_mov_dest = static_cast<pio_src_dest>(3u |
		_PIO_INVALID_IN_SRC | _PIO_INVALID_OUT_DEST | _PIO_INVALID_SET_DEST | _PIO_INVALID_MOV_SRC);
	if (Is_MOV(inst) && ::strcasecmp(str, "pindirs") == 0) {
		if (program_.pio_version < 1) ::panic("addr%02x: pindirs is not supported in PIO version %d\n", relAddrCur_, program_.pio_version);
		return pio_pindirs_mov_dest;
	}
	pio_src_dest dest = StrToSrcDest(inst, str);
	if (Is_MOV(inst) && (dest & _PIO_INVALID_MOV_DEST) != 0) ::panic("addr%02x: invalid destination '%s' for MOV instruction\n", relAddrCur_, str);
	if (Is_OUT(inst) && (dest & _PIO_INVALID_OUT_DEST) != 0) ::panic("addr%02x: invalid destination '%s' for OUT instruction\n", relAddrCur_, str);
	if (Is_SET(inst) && (dest & _PIO_INVALID_SET_DEST) != 0) ::panic("addr%02x: invalid destination '%s' for SET instruction\n", relAddrCur_, str);
	return dest;
}

pio_src_dest Program::StrToSrcDest(uint16_t inst, const char* str) const
{
	if (::strcasecmp(str, "pins") == 0)		return pio_pins;
	if (::strcasecmp(str, "x") == 0)		return pio_x;
	if (::strcasecmp(str, "y") == 0)		return pio_y;
	if (::strcasecmp(str, "pindirs") == 0)	return pio_pindirs;
	if (::strcasecmp(str, "null") == 0)		return pio_null;
	if (::strcasecmp(str, "exec") == 0)		return Is_OUT(inst)? pio_exec_out : pio_exec_mov;
	if (::strcasecmp(str, "status") == 0)	return pio_status;
	if (::strcasecmp(str, "pc") == 0)		return pio_pc;
	if (::strcasecmp(str, "isr") == 0)		return pio_isr;
	if (::strcasecmp(str, "osr") == 0)		return pio_osr;
	::panic("addr%02x: invalid source/destination '%s'\n", relAddrCur_, str);
	return pio_pins;
}

}

}
