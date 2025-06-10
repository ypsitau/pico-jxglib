//==============================================================================
// jxglib/DS323x.h
//==============================================================================
#ifndef PICO_JXGLIB_DS323X_H
#define PICO_JXGLIB_DS323X_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/RTC.h"

namespace jxglib {

//------------------------------------------------------------------------------
// DS323x
//------------------------------------------------------------------------------
class DS323x : public RTC {
private:
	i2c_inst_t* i2c_;
	uint8_t addr_;
public:
	DS323x(i2c_inst_t* i2c, uint8_t addr = 0x68);
public:
	virtual bool DoGet(DateTime* pDt) override;
	virtual bool DoSet(const DateTime &dt) override;
private:
	static uint8_t BCD2Dec(uint8_t val);
	static uint8_t Dec2BCD(uint8_t val);
};

} // namespace jxglib

#endif
