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
// PIO::Resource
//------------------------------------------------------------------------------
bool Resource::Claim(const Program& program)
{
	return ::pio_claim_free_sm_and_add_program(program, &pio, &sm, &offset);
}

bool Resource::Claim(const Program& program, uint gpio_base, uint gpio_count, bool set_gpio_base)
{
	return ::pio_claim_free_sm_and_add_program_for_gpio_range(program, &pio, &sm, &offset, gpio_base, gpio_count, set_gpio_base);
}

void Resource::Remove()
{
}

//------------------------------------------------------------------------------
// PIO::Config
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PIO::StatemMachine
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PIO::Block
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PIO::Program
//------------------------------------------------------------------------------
Program::Program() : name_{""}, addrRelCur_{0}, directive_{Directive::None},
		sideSpecifiedFlag_{false}, wrap_{0, 0}, sideSet_{0, false, false}
{
	for (int i = 0; i < count_of(instTbl_); ++i) instTbl_[i] = 0x0000;
	program_.instructions = instTbl_;;
	program_.length = 0;
	program_.origin = -1; // required instruction memory origin or -1
	program_.pio_version = 0;
#if PICO_PIO_VERSION > 0
	program_.used_gpio_ranges = 0; // bitmap with one bit per 16 pins
#endif
}

Program& Program::AddInst(uint16_t inst)
{
	directive_ = Directive::None; // reset directive
	if (addrRelCur_ > 0 && IsSideMust() && !sideSpecifiedFlag_) {
		::panic("addr%02x: side-set must be specified for each instruction\n", addrRelCur_);
	}
	if (addrRelCur_ >= count_of(instTbl_)) ::panic("addr%02x: too many PIO instructions\n", addrRelCur_);
	instTbl_[addrRelCur_++] = inst;
	sideSpecifiedFlag_ = false;
	return *this;
}

Program& Program::L(const char* label)
{
	if (Lookup(label)) ::panic("addr%02x: label '%s' already defined\n", addrRelCur_, label);
	AddVariable(label, addrRelCur_);
	return *this;
}

void Program::AddVariable(const char* label, uint16_t addrRel)
{
	Variable* pVariable = new Variable(label, addrRel);
	pVariable->SetNext(pVariableHead_.release());
	pVariableHead_.reset(pVariable);
}

void Program::AddVariableRef(const char* label, uint16_t addrRel)
{
	Variable* pVariable = new Variable(label, addrRel);
	pVariable->SetNext(pVariableRefHead_.release());
	pVariableRefHead_.reset(pVariable);
}

const Program::Variable* Program::Lookup(const char* label) const
{
	for (const Variable* pVariable = pVariableHead_.get(); pVariable; pVariable = pVariable->GetNext()) {
		if (::strcmp(pVariable->GetLabel(), label) == 0) return pVariable;
	}
	return nullptr;
}

Program& Program::Complete()
{
	for (Variable* pVariableRef = pVariableRefHead_.get(); pVariableRef; pVariableRef = pVariableRef->GetNext()) {
		const Variable* pVariable = Lookup(pVariableRef->GetLabel());
		if (pVariable) {
			uint16_t& inst = instTbl_[pVariableRef->GetValue()];
			inst = inst & 0xffe0 | pVariable->GetValue();
		} else {
			::panic("addr%02x: label '%s' not found\n", addrRelCur_, pVariableRef->GetLabel());
		}
	}
	program_.length = addrRelCur_;
	if (wrap_.addrRel_wrap == 0) wrap_.addrRel_wrap = addrRelCur_;
	return *this;
}

pio_sm_config Program::GenerateConfig(uint offset) const
{
	pio_sm_config config = ::pio_get_default_sm_config();
	::sm_config_set_sideset(&config, sideSet_.bit_count, sideSet_.optional, sideSet_.pindirs);
	::sm_config_set_wrap(&config, offset + wrap_.addrRel_target, offset + wrap_.addrRel_wrap);
	return config;	
}

const Program& Program::Dump() const
{
	for (uint16_t addrRel = 0; addrRel < addrRelCur_; ++addrRel) {
		uint16_t inst = instTbl_[addrRel];
		::printf("%02x  %03b %05b %03b %05b\n", addrRel,
			(inst >> 13) & 0b111, (inst >> 8) & 0b11111, (inst >> 5) & 0b111, (inst >> 0) & 0b11111);
	}
	return *this;
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
	::panic("addr%02x: jmp(): invalid condition '%s'\n", addrRelCur_, cond);
	return *this;
}

Program& Program::wait(bool polarity, const char* src, uint16_t index)
{
	if (::strcasecmp(src, "gpio") == 0)		return wait_gpio(polarity, index);
	if (::strcasecmp(src, "pin") == 0)		return wait_pin(polarity, index);
	if (::strcasecmp(src, "irq") == 0)		return wait_irq(polarity, false, index);
	if (::strcasecmp(src, "jmppin") == 0) {
		if (program_.pio_version < 1) ::panic("addr%02x: wait(): jmppin is not supported in PIO version %d\n", addrRelCur_, program_.pio_version);
		return wait_jmppin(polarity, index);
	}
	::panic("addr%02x: wait(): invalid source '%s'\n", addrRelCur_, src);
	return *this;
}

Program& Program::mov(const char* dest, const char* src, uint16_t index)
{
	if (StartsWithICase(dest, "rxfifo[")) {
		if (program_.pio_version < 1) ::panic("addr%02x: mov(): rxfifo[] is not supported in PIO version %d\n", addrRelCur_, program_.pio_version);
		dest += 7;
		if (::strcasecmp(src, "isr") != 0) ::panic("addr%02x: mov(): invalid source '%s'\n", addrRelCur_, src);
		uint16_t inst = 0;
		if (::strcasecmp(dest, "y]") == 0) {
			inst = 0b10000000'00010000;
		} else if (::strcasecmp(dest, "]") == 0) {
			inst = 0b10000000'00011000 | index;
		} else {
			char* endptr;
			int index = ::strtol(dest, &endptr, 0);
			if (::strcmp(endptr, "]") != 0) ::panic("addr%02x: mov(): invalid destination '%s'\n", addrRelCur_, dest);
			if (index < 0 || index > 7) ::panic("addr%02x: mov(): index out of range %d\n", addrRelCur_, index);
			inst = 0b10000000'00011000 | static_cast<uint16_t>(index);
		}
		return word(inst);
	} else if (StartsWithICase(src, "rxfifo[")) {
		if (program_.pio_version < 1) ::panic("addr%02x: mov(): rxfifo[] is not supported in PIO version %d\n", addrRelCur_, program_.pio_version);
		src += 7;
		if (::strcasecmp(dest, "osr") != 0) ::panic("addr%02x: mov(): invalid destination '%s'\n", addrRelCur_, dest);
		uint16_t inst = 0;
		if (::strcasecmp(src, "y]") == 0) {
			inst = 0b10000000'10010000;
		} else if (::strcasecmp(src, "]") == 0) {
			inst = 0b10000000'10011000 | index;
		} else {
			char* endptr;
			int index = ::strtol(src, &endptr, 0);
			if (::strcmp(endptr, "]") != 0) ::panic("addr%02x: mov(): invalid source '%s'\n", addrRelCur_, src);
			if (index < 0 || index > 7) ::panic("addr%02x: mov(): index out of range %d\n", addrRelCur_, index);
			inst = 0b10000000'10011000 | static_cast<uint16_t>(index);
		}
		return word(inst);
	} else {
		if (*src == '!' || *src == '~') return mov_not(StrToSrcDest(dest), StrToSrcDest(src + 1));
		if (*src == ':' && *(src + 1) == ':') return mov_reverse(StrToSrcDest(dest), StrToSrcDest(src + 2));
		return mov(StrToSrcDest(dest), StrToSrcDest(src));
	}
}

Program& Program::irq(const char* op, uint16_t irq_n)
{
	bool relative = false;
	if (::strcasecmp(op, "set") == 0) return irq_set(irq_n, relative);
	if (::strcasecmp(op, "nowait") == 0) return irq_set(irq_n, relative);
	if (::strcasecmp(op, "wait") == 0) return irq_wait(irq_n, relative);
	if (::strcasecmp(op, "clear") == 0) return irq_clear(irq_n, relative);
	::panic("addr%02x: irq(): invalid operation '%s'\n", addrRelCur_, op);
	return *this;
}

Program& Program::opt()
{
	if (directive_ == Directive::side_set) {
		sideSet_.optional = true;
	} else {
		::panic("addr%02x: opt() is only applicable for side_set\n", addrRelCur_);
	}
	return *this;
}

Program& Program::pindirs()
{
	if (directive_ == Directive::side_set) {
		sideSet_.pindirs = true;
	} else {
		::panic("addr%02x: pindirs() is only applicable for side_set\n", addrRelCur_);
	}
	return *this;
}

Program& Program::side(uint16_t bits)
{
	if (sideSet_.bit_count == 0) ::panic("addr%02x: side(): side-set not defined\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	int lsb = 13 - sideSet_.bit_count;
	inst = inst | (bits << lsb);
	sideSpecifiedFlag_ = true;
	return *this;
}

Program& Program::delay(uint16_t cycles)
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	inst = inst | (cycles << 8);
	return *this;
}

Program& Program::rel()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_IRQ(inst)) {
		inst |= (2 << 3);
	} else if (Is_WAIT(inst) && ((inst >> 5) & 0x3) == 2) {	// wait irq
		inst |= (2 << 3);
	} else {
		::panic("addr%02x: rel() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::prev()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_IRQ(inst)) {
		inst |= (1 << 3);
	} else if (Is_WAIT(inst) && ((inst >> 5) & 0x3) == 2) {	// wait irq
		inst |= (1 << 3);
	} else {
		::panic("addr%02x: prev() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::next()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_IRQ(inst)) {
		inst |= (3 << 3);
	} else if (Is_WAIT(inst) && ((inst >> 5) & 0x3) == 2) {	// wait irq
		inst |= (3 << 3);
	} else {
		::panic("addr%02x: next() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::iffull()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_PUSH(inst)) {
		inst |= (1 << 6);
	} else {
		::panic("addr%02x: iffull() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::ifempty()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_PULL(inst)) {
		inst |= (1 << 6);
	} else {
		::panic("addr%02x: iffull() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::block()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_PUSHorPULL(inst)) {
		inst |= (1 << 5);
	} else {
		::panic("addr%02x: block() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
	}
	return *this;
}

Program& Program::noblock()
{
	if (addrRelCur_ == 0 || directive_ != Directive::None) ::panic("addr%02x: no proceeding instruction\n", addrRelCur_);
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	if (Is_PUSHorPULL(inst)) {
		inst &= ~(1 << 5);
	} else {
		::panic("addr%02x: block() is not applicable for %s\n", addrRelCur_, GetInstName(inst));
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

pio_src_dest Program::StrToSrcDest(const char* str, bool outFlag) const
{
	if (::strcasecmp(str, "pins") == 0)		return pio_pins;
	if (::strcasecmp(str, "x") == 0)		return pio_x;
	if (::strcasecmp(str, "y") == 0)		return pio_y;
	if (::strcasecmp(str, "null") == 0)		return pio_null;
	if (::strcasecmp(str, "pindirs") == 0)	return pio_pindirs;
	if (::strcasecmp(str, "exec") == 0)		return outFlag? pio_exec_out : pio_exec_mov;
	if (::strcasecmp(str, "status") == 0)	return pio_status;
	if (::strcasecmp(str, "pc") == 0)		return pio_pc;
	if (::strcasecmp(str, "isr") == 0)		return pio_isr;
	if (::strcasecmp(str, "osr") == 0)		return pio_osr;
	::panic("addr%02x: invalid source/destination '%s'\n", addrRelCur_, str);
	return pio_pins;
}

}

}
