//==============================================================================
// Pico.cpp
//==============================================================================
#include "hardware/rtc.h"
#include "jxglib/RTC/Pico.h"

namespace jxglib::RTC {

//------------------------------------------------------------------------------
// Pico
//------------------------------------------------------------------------------
Pico::Pico() : Base("Pico"), initializedFlag_{false}
{
}

bool Pico::DoGet(DateTime* pDt)
{
	if (!initializedFlag_) {
		::rtc_init();
		initializedFlag_ = true;
	}
	datetime_t dtSDK;
	if (::rtc_get_datetime(&dtSDK)) {
		*pDt = DateTime(
			dtSDK.year,
			dtSDK.month,
			dtSDK.day,
			dtSDK.hour,
			dtSDK.min,
			dtSDK.sec,
			0
		);
	} else {
		// RTC is not running. Set a default value to start with.
		DateTime dt;
		DoSet(dt);
	}
	return true;
}

bool Pico::DoSet(const DateTime &dt)
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
