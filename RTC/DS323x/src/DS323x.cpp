//==============================================================================
// DS323x.cpp
//==============================================================================
#include "jxglib/RTC/DS323x.h"

namespace jxglib::RTC {

//------------------------------------------------------------------------------
// DS323x
//------------------------------------------------------------------------------
DS323x::DS323x(i2c_inst_t* i2c, uint8_t addr) : Base("DS323x"), i2c_(i2c), addr_(addr)
{
}

bool DS323x::DoGet(DateTime* pDt)
{
	uint8_t reg = 0x00;
	if (::i2c_write_blocking(i2c_, addr_, &reg, sizeof(reg), true) != sizeof(reg)) return false;
	uint8_t buf[7];
	if (::i2c_read_blocking(i2c_, addr_, buf, sizeof(buf), false) != sizeof(buf)) return false;
	pDt->msec = 0; // DS323x does not support milliseconds
	pDt->sec	= BCD2Dec(buf[0]);
	pDt->min	= BCD2Dec(buf[1]);
	pDt->hour	= BCD2Dec(buf[2]);
	// buf[3] is weekday, not used here
	pDt->day	= BCD2Dec(buf[4]);
	pDt->month	= BCD2Dec(buf[5]);
	pDt->year	= 2000 + BCD2Dec(buf[6]);
	return true;
}

bool DS323x::DoSet(const DateTime &dt)
{
	uint8_t buf[8];
	buf[0] = 0x00; // start register address
	buf[1] = Dec2BCD(dt.sec);
	buf[2] = Dec2BCD(dt.min);
	buf[3] = Dec2BCD(dt.hour);
	buf[4] = 0; // weekday (not set)
	buf[5] = Dec2BCD(dt.day);
	buf[6] = Dec2BCD(dt.month);
	buf[7] = Dec2BCD(dt.year - 2000);
	return ::i2c_write_blocking(i2c_, addr_, buf, sizeof(buf), false) == sizeof(buf);
}

}
