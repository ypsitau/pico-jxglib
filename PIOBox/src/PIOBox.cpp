//==============================================================================
// PIOBox.cpp
//==============================================================================
#include "jxglib/PIOBox.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PIOIf
//------------------------------------------------------------------------------
PIOIf PIO0(pio0);
PIOIf PIO1(pio1);

//------------------------------------------------------------------------------
// PIOProgram
//------------------------------------------------------------------------------
PIOProgram::PIOProgram() : addrRelCur_{0}, addrRel_wrap_{AddrRelInvalid}, addrRel_wrap_target_{AddrRelInvalid}, sideSet_{0, false, false}
{
	for (int i = 0; i < count_of(instTbl_); ++i) instTbl_[i] = 0x0000;
}

void PIOProgram::AddVariable(const char* label, uint16_t addrRel)
{
	Variable* pVariable = new Variable(label, addrRel);
	pVariable->SetNext(pVariableHead_.release());
	pVariableHead_.reset(pVariable);
}

void PIOProgram::AddVariableRef(const char* label, uint16_t addrRel)
{
	Variable* pVariable = new Variable(label, addrRel);
	pVariable->SetNext(pVariableRefHead_.release());
	pVariableRefHead_.reset(pVariable);
}

const PIOProgram::Variable* PIOProgram::LookupVariable(const char* label) const
{
	for (const Variable* pVariable = pVariableHead_.get(); pVariable; pVariable = pVariable->GetNext()) {
		if (::strcmp(pVariable->GetLabel(), label) == 0) return pVariable;
	}
	return nullptr;
}

void PIOProgram::Resolve()
{
	for (Variable* pVariableRef = pVariableRefHead_.get(); pVariableRef; pVariableRef = pVariableRef->GetNext()) {
		const Variable* pVariable = LookupVariable(pVariableRef->GetLabel());
		if (pVariable) {
			uint16_t& inst = instTbl_[pVariableRef->GetAddrRel()];
			inst = inst & 0xffe0 | pVariable->GetAddrRel();
		} else {
			::panic("PIOProgram::ResolveLabelRef: label '%s' not found\n", pVariableRef->GetLabel());
		}
	}
}

PIOProgram& PIOProgram::AddInst(uint16_t inst)
{
	if (addrRelCur_ < count_of(instTbl_)) {
		instTbl_[addrRelCur_++] = inst;
	} else {
		::panic("PIOProgram::AddInst: too many PIO instructions\n");
	}
	return *this;
}

PIOProgram& PIOProgram::L(const char* label)
{
	return *this;
}

PIOProgram& PIOProgram::wrap_target()
{
	addrRel_wrap_target_ = addrRelCur_;
	return *this;
}

PIOProgram& PIOProgram::wrap()
{
	addrRel_wrap_ = addrRelCur_;
	return *this;
}

PIOProgram& PIOProgram::side_set(int count, bool optFlag, bool pindirsFlag)
{
	sideSet_.count = count;
	sideSet_.optFlag = optFlag;
	sideSet_.pindirsFlag = pindirsFlag;
	return *this;
}

PIOProgram& PIOProgram::jmp(const char* cond, uint16_t addr)
{
	if (::strcasecmp(cond, "!x") == 0) return jmp_not_x(addr);
	if (::strcasecmp(cond, "x--") == 0) return jmp_x_dec(addr);
	if (::strcasecmp(cond, "!y") == 0) return jmp_not_y(addr);
	if (::strcasecmp(cond, "y--") == 0) return jmp_y_dec(addr);
	if (::strcasecmp(cond, "x!=y") == 0 || ::strcasecmp(cond, "x != y") == 0) return jmp_x_ne_y(addr);
	if (::strcasecmp(cond, "pin") == 0) return jmp_pin(addr);
	if (::strcasecmp(cond, "!osre") == 0) return jmp_not_osre(addr);
	::panic("PIOProgram::jmp: invalid condition '%s'\n", cond);
	return *this;
}

PIOProgram& PIOProgram::irq(const char* op, uint16_t irq_n, bool relative)
{
	if (::strcasecmp(op, "set") == 0) return irq_set(irq_n, relative);
	if (::strcasecmp(op, "wait") == 0) return irq_wait(irq_n, relative);
	if (::strcasecmp(op, "clear") == 0) return irq_clear(irq_n, relative);
	::panic("PIOProgram::irq: invalid operation '%s'\n", op);
	return *this;
}

PIOProgram& PIOProgram::side(uint16_t bits)
{
	if (sideSet_.count == 0) ::panic("PIOProgram::side: side-set not defined\n");
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	int lsb = 13 - sideSet_.count;
	inst = inst | (bits << lsb);
	return *this;
}

PIOProgram& PIOProgram::delay(uint16_t cycles)
{
	uint16_t& inst = instTbl_[addrRelCur_ - 1];
	inst = inst | (cycles << 8);
	return *this;
}

//------------------------------------------------------------------------------
// PIOBox
//------------------------------------------------------------------------------
bool PIOBox::ClaimResource()
{
	PIO pio_;
	uint sm_;
	if (!PIOIf::claim_free_sm_and_add_program(program, &pio_, &sm_, &offset)) return false;
	pio = PIOIf(pio_);
	sm.SetResource(pio_, sm_);
	cfg = get_default_config(offset);
	uint pin = 0;
	pio.gpio_init(pin);
	sm.set_consecutive_pindirs_in(pin, 1);
	sm.set_consecutive_pindirs_out(pin, 1);
	cfg.set_set_pins(pin, 1);
	return true;
}

void PIOBox::UnclaimResource()
{
	PIOIf::remove_program_and_unclaim_sm(program, sm.GetPIO(), sm.GetSM(), offset);
	sm.Invalidate();
}

int PIOBox::InitSM()
{
	return sm.init(offset, cfg);
}

}
