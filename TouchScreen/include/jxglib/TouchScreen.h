//==============================================================================
// jxglib/TouchScreen.h
//==============================================================================
#ifndef PICO_JXGLIB_TOUCHSCREEN_H
#define PICO_JXGLIB_TOUCHSCREEN_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TouchScreen
//------------------------------------------------------------------------------
class TouchScreen {
public:
	TouchScreen() {}
public:
	virtual bool ReadPosition(int* px, int* py) = 0;
	virtual bool IsTouched() = 0;
};

}

#endif
