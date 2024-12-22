//==============================================================================
// Rect.cpp
//==============================================================================
#include "jxglib/Util.h"
#include "jxglib/Rect.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Rect
//------------------------------------------------------------------------------
const Rect Rect::Zero { 0, 0, 0, 0 };
const Rect Rect::Empty { 0, 0, 0, 0 };

Rect Rect::Inflate(int amount) const
{
	int xNew = x, yNew = y, widthNew = width, heightNew = height;
	int amount2 = amount * 2;
	if (widthNew + amount2 < 0) {
		xNew += widthNew / 2;
		widthNew = 0;
	} else {
		xNew -= amount;
		widthNew += amount2;
	}
	if (heightNew + amount2 < 0) {
		yNew += heightNew / 2;
		heightNew = 0;
	} else {
		yNew -= amount;
		heightNew += amount2; 
	}
	Rect rect {xNew, yNew, widthNew, heightNew};
	return rect;
}

bool Rect::Adjust(const Rect& rectBound)
{
	return AdjustRange(&x, &width, rectBound.x, rectBound.x + rectBound.width) &&
        AdjustRange(&y, &height, rectBound.y, rectBound.y + rectBound.height);
}

}
