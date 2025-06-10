//==============================================================================
// DateTime.cpp
//==============================================================================
#include <ctype.h>
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

bool DateTime::Parse(const char* str)
{
	int16_t year_tmp = 0;
	int8_t month_tmp = 0, day_tmp = 0;
	const char* p = str;
	// Skip leading spaces
	for ( ; ::isspace(*p); ++p) ;
	// Parse year (4 digits)
	do {
		int digits = 0;
		for ( ; digits < 4 && ::isdigit(*p); ++digits, ++p) year_tmp = year_tmp * 10 + (*p - '0');
		if (*p != '-' && *p != '/') return false;
	} while (0);
	char sep = *p++;
	// Parse month (1 or 2 digits)
	do {
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) month_tmp = month_tmp * 10 + (*p - '0');
		if (digits == 0 || *p != sep) return false;
		++p;
	} while (0);
	// Parse day (1 or 2 digits)
	do {
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) day_tmp = day_tmp * 10 + (*p - '0');
		if (digits == 0) return false;
	} while (0);
	// Optional time part
	if (::isspace(*p) || *p == 'T') {
		++p;
		if (!ParseTime(p)) return false;
	} else {
		hour = min = sec = msec = 0;
	}
	year = year_tmp;
	month = month_tmp;
	day = day_tmp;
	return true;
}

// Make ParseTime a DateTime member function
bool DateTime::ParseTime(const char* str)
{
	int8_t hour_tmp = 0, min_tmp = 0, sec_tmp = 0;
	int16_t msec_tmp = 0;
	const char* p = str;
	// Skip leading spaces
	for ( ; ::isspace(*p); ++p) ;
	// hour
	do {
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) hour_tmp = hour_tmp * 10 + (*p - '0');
		if (digits == 0 || *p++ != ':') return false;
	} while (0);
	// minute
	do {
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) min_tmp = min_tmp * 10 + (*p - '0');
		if (digits == 0) return false;
	} while (0);
	// Optional second
	bool secondPresentFlag;
	if (secondPresentFlag = (*p == ':')) {
		++p;
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) sec_tmp = sec_tmp * 10 + (*p - '0');
		if (digits == 0) return false;
	}
	// Optional .msec
	if (secondPresentFlag && (*p == '.' || *p == ',')) {
		++p;
		int digits = 0;
		for ( ; digits < 3 && ::isdigit(*p); ++digits, ++p) msec_tmp = msec_tmp * 10 + (*p - '0');
		if (digits == 0) return false;
		// If less than 3 digits, scale
		while (digits++ < 3) msec_tmp *= 10;
	}
	if (!::isspace(*p) && *p != '\0') return false;
	hour = hour_tmp;
	min = min_tmp;
	sec = sec_tmp;
	msec = msec_tmp;
	return true;
}

}
