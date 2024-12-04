//==============================================================================
// PIO.cpp
//==============================================================================
#include "jxglib/PIO.h"

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
	PIO pio;
	uint sm;
	if (!PIOIf::claim_free_sm_and_add_program(program, &pio, &sm, &offset)) return false;
	this->sm.SetResource(pio, sm);
	return true;
}

bool PIOBox::ClaimResource(uint gpio_base, uint gpio_count, bool set_gpio_base)
{
	PIO pio;
	uint sm;
	if (!PIOIf::claim_free_sm_and_add_program_for_gpio_range(program, &pio, &sm, &offset, gpio_base, gpio_count, set_gpio_base)) return false;
	this->sm.SetResource(pio, sm);
	return true;
}

void PIOBox::UnclaimResource()
{
	PIOIf::remove_program_and_unclaim_sm(program, sm.GetPIO(), sm.GetSM(), offset);
	sm.Invalidate();
}

int PIOBox::Init(const PIOIf::Config& cfg)
{
	return sm.init(offset, cfg);
}

}
