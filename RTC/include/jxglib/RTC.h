//==============================================================================
// jxglib/RTC.h
//==============================================================================
#ifndef PICO_JXGLIB_RTC_H
#define PICO_JXGLIB_RTC_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// RTC
//------------------------------------------------------------------------------
class RTC {
private:
	static RTC* pInstance;
public:
	RTC() {}
public:
	virtual bool DoSet(const DateTime& dt) = 0;
	virtual bool DoGet(DateTime* pDt) = 0;
};

//------------------------------------------------------------------------------
// RTCDummy
//------------------------------------------------------------------------------
class RTCDummy {
public:
	static RTCDummy Instance;
public:
	RTCDummy() {}
public:
	virtual bool DoSet(const DateTime& dt) { return true; }
	virtual bool DoGet(DateTime* pDt) { return true; }
};

}

#endif
