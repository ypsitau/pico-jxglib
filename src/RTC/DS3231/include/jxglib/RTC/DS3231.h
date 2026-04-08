//==============================================================================
// jxglib/RTC/DS3231.h
//==============================================================================
#ifndef PICO_JXGLIB_RTC_DS3231_H
#define PICO_JXGLIB_RTC_DS3231_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/RTC.h"

namespace jxglib::RTC {

//------------------------------------------------------------------------------
// DS3231
//------------------------------------------------------------------------------
class DS3231 : public Base {
private:
	i2c_inst_t* i2c_;
	uint32_t msecTimeout_;
	uint8_t addr_;
public:
	DS3231(i2c_inst_t* i2c, uint32_t msecTimeout = 300, uint8_t addr = 0x68);
public:
	virtual bool DoGet(DateTime* pDt) override;
	virtual bool DoSet(const DateTime &dt) override;
};

} // namespace jxglib

#endif
