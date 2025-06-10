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
	static bool Set(const DateTime& dt) { return pInstance->DoSet(dt); }
	static bool Get(DateTime* pDt) { return pInstance->DoGet(pDt); }
	static void SetInstance(RTC& rtc) { pInstance = &rtc; }
public:
	virtual bool DoSet(const DateTime& dt) = 0;
	virtual bool DoGet(DateTime* pDt) = 0;
};

//------------------------------------------------------------------------------
// RTCDummy
//------------------------------------------------------------------------------
class RTCDummy : public RTC {
public:
	static RTCDummy Instance;
public:
	RTCDummy() {}
public:
	virtual bool DoSet(const DateTime& dt) override;
	virtual bool DoGet(DateTime* pDt) override;
};

}

#endif
