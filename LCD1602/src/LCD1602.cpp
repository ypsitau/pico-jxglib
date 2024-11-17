//==============================================================================
// LCD1602.cpp
//==============================================================================
#include "jxglib/LCD1602.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LCD1602
//------------------------------------------------------------------------------
void LCD1602::Initialize()
{
	raw.ReturnHome(1);
	raw.ReturnHome(1);
	raw.ReturnHome(1);
	raw.ReturnHome();
	raw.EntryModeSet(1, 0);
	raw.FunctionSet(0, 1, 0);
	raw.DisplayOnOffControl(1, 0, 0);
	raw.ClearDisplay();
}

}
