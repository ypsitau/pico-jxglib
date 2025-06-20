//==============================================================================
// jxglib/RTC/DS323x.h
//==============================================================================
#ifndef PICO_JXGLIB_RTC_DS323X_H
#define PICO_JXGLIB_RTC_DS323X_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/RTC.h"

namespace jxglib::RTC {

//------------------------------------------------------------------------------
// DS323x
//------------------------------------------------------------------------------
class DS323x : public Base {
private:
	i2c_inst_t* i2c_;
	uint32_t msecTimeout_;
	uint8_t addr_;
public:
	DS323x(i2c_inst_t* i2c, uint32_t msecTimeout = 300, uint8_t addr = 0x68);
public:
	virtual bool DoGet(DateTime* pDt) override;
	virtual bool DoSet(const DateTime &dt) override;
};

} // namespace jxglib

#endif
