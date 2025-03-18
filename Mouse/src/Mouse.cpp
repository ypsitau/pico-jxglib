//==============================================================================
// Mouse.cpp
//==============================================================================
#include "jxglib/Mouse.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Mouse
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Mouse::Status
//------------------------------------------------------------------------------
void Mouse::Status::Update(const Point& pt, int deltaX, int deltaY, int deltaWheel, int deltaPan, uint8_t buttons)
{
	pt_ = pt;
	deltaX_ += deltaX, deltaY_ += deltaY;
	deltaWheel_ += deltaWheel;
	deltaPan_ += deltaPan;
	buttonsPrev_ = buttons_;
	buttons_ = buttons;
}

Mouse::Status Mouse::Status::Capture()
{
	Status statusRtn = *this;
	deltaX_ = 0;
	deltaY_ = 0;
	deltaWheel_ = 0;
	deltaPan_ = 0;
	return statusRtn;
}

}
