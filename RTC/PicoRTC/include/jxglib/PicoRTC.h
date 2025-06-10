//==============================================================================
// jxglib/PicoRTC.h
//==============================================================================
#ifndef PICO_JXGLIB_PICORTC_H
#define PICO_JXGLIB_PICORTC_H
#include "pico/stdlib.h"
#include "jxglib/RTC.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PicoRTC
//------------------------------------------------------------------------------
class PicoRTC : public RTC {
private:
	bool initializedFlag_;
public:
	PicoRTC();
public:
	virtual bool DoGet(DateTime* pDt) override;
	virtual bool DoSet(const DateTime &dt) override;
};

} // namespace jxglib

#endif
