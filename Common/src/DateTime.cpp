//==============================================================================
// DateTime.cpp
//==============================================================================
#include "jxglib/DateTime.h"

namespace jxglib {

//------------------------------------------------------------------------------
// DateTime
//------------------------------------------------------------------------------
const DateTime DateTime::Empty;

int8_t DateTime::CalcDayOfWeek() const
{
    // Zeller's Congruence (0=Saturday, 1=Sunday, ..., 6=Friday)
    int y = year;
    int m = month;
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    int dow = (y + y/4 - y/100 + y/400 + (13 * m + 8) / 5 + day) % 7;
    if (dow < 0) dow += 7;
    // Convert to 0=Sunday, 1=Monday, ..., 6=Saturday
    return dow;
}

DateTime& DateTime::operator=(const DateTime& dt)
{
	if (this != &dt) {
		year = dt.year;
		month = dt.month;
		day = dt.day;
		hour = dt.hour;
		min = dt.min;
		sec = dt.sec;
		msec = dt.msec;
	}
	return *this;
}

int DateTime::Compare(const DateTime& dt1, const DateTime& dt2)
{
	auto Sign = [](int a, int b) { return (a > b) ? 1 : (a < b)? -1 : 0; };
	int rtn;
	if ((rtn = Sign(dt1.year, dt2.year)) != 0) return rtn;
	if ((rtn = Sign(dt1.month, dt2.month)) != 0) return rtn;
	if ((rtn = Sign(dt1.day, dt2.day)) != 0) return rtn;
	if ((rtn = Sign(dt1.hour, dt2.hour)) != 0) return rtn;
	if ((rtn = Sign(dt1.min, dt2.min)) != 0) return rtn;
	if ((rtn = Sign(dt1.sec, dt2.sec)) != 0) return rtn;
	if ((rtn = Sign(dt1.msec, dt2.msec)) != 0) return rtn;
	return 0;
}

}
