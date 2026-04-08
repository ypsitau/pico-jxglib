//==============================================================================
// jxglib/Pico.h
//==============================================================================
#ifndef PICO_JXGLIB_PICO_H
#define PICO_JXGLIB_PICO_H
#include "pico/stdlib.h"
#include "jxglib/RTC.h"

namespace jxglib::RTC {

//------------------------------------------------------------------------------
// Pico
//------------------------------------------------------------------------------
class Pico : public Base {
private:
	bool initializedFlag_;
public:
	Pico();
public:
	virtual bool DoGet(DateTime* pDt) override;
	virtual bool DoSet(const DateTime &dt) override;
};

} // namespace jxglib

#endif
