//==============================================================================
// jxglib/RTC.h
//==============================================================================
#ifndef PICO_JXGLIB_RTC_H
#define PICO_JXGLIB_RTC_H
#include "pico/stdlib.h"
#include "jxglib/DateTime.h"

namespace jxglib::RTC {

class Base;
extern Base* pInstance;

//------------------------------------------------------------------------------
// Base
//------------------------------------------------------------------------------
class Base {
public:
	Base();
public:
	virtual bool DoSet(const DateTime& dt) = 0;
	virtual bool DoGet(DateTime* pDt) = 0;
public:
	static uint8_t BCD2Dec(uint8_t val) { return (val >> 4) * 10 + (val & 0x0F); }
	static uint8_t Dec2BCD(uint8_t val) { return ((val / 10) << 4) | (val % 10); }
};

//------------------------------------------------------------------------------
// Dummy
//------------------------------------------------------------------------------
class Dummy : public Base {
public:
	static Dummy Instance;
private:
	DateTime dt_;
public:
	Dummy() {}
public:
	virtual bool DoSet(const DateTime& dt) override;
	virtual bool DoGet(DateTime* pDt) override;
};

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
inline bool Set(const DateTime& dt) { return pInstance->DoSet(dt); }
inline bool Get(DateTime* pDt) { return pInstance->DoGet(pDt); }

}

#endif
