//==============================================================================
// PicoRTC.cpp
//==============================================================================
#include "hardware/rtc.h"
#include "jxglib/PicoRTC.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PicoRTC
//------------------------------------------------------------------------------
PicoRTC::PicoRTC() : initializedFlag_{false}
{
}

bool PicoRTC::DoGet(DateTime* pDt)
{
	if (!initializedFlag_) {
		::rtc_init();
		initializedFlag_ = true;
	}
	datetime_t dtSDK;
	if (!::rtc_get_datetime(&dtSDK)) return false;
	*pDt = DateTime(
		dtSDK.year,
		dtSDK.month,
		dtSDK.day,
		dtSDK.hour,
		dtSDK.min,
		dtSDK.sec,
		0
	);
	return true;
}

bool PicoRTC::DoSet(const DateTime &dt)
{
	if (!initializedFlag_) {
		::rtc_init();
		initializedFlag_ = true;
	}
	datetime_t dtSDK;
	dtSDK = {
		year:	dt.year,
		month:	dt.month,
		day:	dt.day,
		dotw:	dt.CalcDayOfWeek(),
		hour:	dt.hour,
		min:	dt.min,
		sec:	dt.sec
	};	
	return ::rtc_set_datetime(&dtSDK);
}

}
