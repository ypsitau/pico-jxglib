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
	bool ReadPoint(Point* pPt) { return ReadXY(&pPt->x, &pPt->y); }
public:
	virtual bool ReadXY(int* px, int* py) = 0;
	virtual bool IsTouched() = 0;
};

}

#endif
