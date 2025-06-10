//==============================================================================
// RTC.cpp
//==============================================================================
#include "jxglib/RTC.h"

namespace jxglib {

//------------------------------------------------------------------------------
// RTC
//------------------------------------------------------------------------------
RTC* RTC::pInstance = &RTCDummy::Instance;

RTC::RTC()
{
	pInstance = this; // Set the static instance pointer to this instance	
}

//------------------------------------------------------------------------------
// RTCDummy
//------------------------------------------------------------------------------
RTCDummy RTCDummy::Instance;

bool RTCDummy::DoSet(const DateTime& dt)
{
	dt_ = dt;
	return true;
}

bool RTCDummy::DoGet(DateTime* pDt)
{
	*pDt = dt_;
	return true;
}

}
