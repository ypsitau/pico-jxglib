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
// PIOContext
//------------------------------------------------------------------------------
bool PIOContext::ClaimResource()
{
	PIO pio;
	uint sm;
	if (!PIOIf::claim_free_sm_and_add_program(program_, &pio, &sm, &offset_)) return false;
	stateMachine_.SetResource(pio, sm);
	return true;
}

bool PIOContext::ClaimResource(uint gpio_base, uint gpio_count, bool set_gpio_base)
{
	PIO pio;
	uint sm;
	if (!PIOIf::claim_free_sm_and_add_program_for_gpio_range(program_, &pio, &sm, &offset_, gpio_base, gpio_count, set_gpio_base)) return false;
	stateMachine_.SetResource(pio, sm);
	return true;
}

void PIOContext::UnclaimResource()
{
	PIOIf::remove_program_and_unclaim_sm(program_, stateMachine_.Get_pio(), stateMachine_.Get_sm(), offset_);
	stateMachine_.Invalidate();
}

}
