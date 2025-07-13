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
