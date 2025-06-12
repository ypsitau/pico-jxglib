//==============================================================================
// RTC.cpp
//==============================================================================
#include "jxglib/RTC.h"

namespace jxglib::RTC {

Base* pInstance = &Dummy::Instance;

//------------------------------------------------------------------------------
// Base
//------------------------------------------------------------------------------
Base::Base()
{
	pInstance = this; // Set the static instance pointer to this instance	
}

//------------------------------------------------------------------------------
// Dummy
//------------------------------------------------------------------------------
Dummy Dummy::Instance;

bool Dummy::DoSet(const DateTime& dt)
{
	dt_ = dt;
	return true;
}

bool Dummy::DoGet(DateTime* pDt)
{
	*pDt = dt_;
	return true;
}

}
