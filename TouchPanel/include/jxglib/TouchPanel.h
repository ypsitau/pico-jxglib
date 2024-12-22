//==============================================================================
// jxglib/TouchPanel.h
//==============================================================================
#ifndef PICO_JXGLIB_TOUCHPANEL_H
#define PICO_JXGLIB_TOUCHPANEL_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TouchPanel
//------------------------------------------------------------------------------
class TouchPanel {
public:
	TouchPanel() {}
public:
	virtual bool ReadPosition(int* px, int* py) = 0;
	virtual bool IsTouched() = 0;
};

}

#endif
