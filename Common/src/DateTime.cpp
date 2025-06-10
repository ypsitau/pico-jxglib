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
	const char* p = str;
	if (!ParseDate(p, &p)) return false;
	if (::isspace(*p) || *p == 'T') {	// Optional time part
		++p;
		if (!ParseTime(p)) return false;
	} else {
		hour = min = sec = msec = 0;
	}
	return true;
}

bool DateTime::ParseDate(const char* str, const char** endPtr)
{
	if (endPtr) *endPtr = nullptr;	// Initialize endPtr to nullptr
	int16_t year_tmp = 0;
	int8_t month_tmp = 0, day_tmp = 0;
	const char* p = str;
	for ( ; ::isspace(*p); ++p) ;	// Skip leading spaces
	do {	// Parse year (4 digits)
		int digits = 0;
		for ( ; digits < 4 && ::isdigit(*p); ++digits, ++p) year_tmp = year_tmp * 10 + (*p - '0');
		if (*p != '-' && *p != '/') return false;
	} while (0);
	char sep = *p++;
	do {	// Parse month (1 or 2 digits)
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) month_tmp = month_tmp * 10 + (*p - '0');
		if (digits == 0 || *p != sep) return false;
		++p;
	} while (0);
	do {	// Parse day (1 or 2 digits)
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) day_tmp = day_tmp * 10 + (*p - '0');
		if (digits == 0) return false;
	} while (0);
	if (endPtr) {
		*endPtr = p;	// Set endPtr to the position after the date
	} else if (!::isspace(*p) && *p != '\0') {
		return false;	// If not end of string, it must be a space
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
	for ( ; ::isspace(*p); ++p) ;	// Skip leading spaces
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
	
	bool secondPresentFlag = false;;
	if (secondPresentFlag = (*p == ':')) {	// Optional second, optional .msec
		++p;
		int digits = 0;
		for ( ; digits < 2 && ::isdigit(*p); ++digits, ++p) sec_tmp = sec_tmp * 10 + (*p - '0');
		if (digits == 0) return false;
	}
	
	if (secondPresentFlag && (*p == '.' || *p == ',')) {
		++p;
		int digits = 0;
		for ( ; digits < 3 && ::isdigit(*p); ++digits, ++p) msec_tmp = msec_tmp * 10 + (*p - '0');
		if (digits > 0) {
			while (digits++ < 3) msec_tmp *= 10;
			return false;
		}
	}
	if (!::isspace(*p) && *p != '\0') return false;
	hour = hour_tmp;
	min = min_tmp;
	sec = sec_tmp;
	msec = msec_tmp;
	return true;
}

bool DateTime::IsTime(const char* str)
{
	const char* p = str;
	for ( ; ::isspace(*p); ++p) ;	// Skip leading spaces
	if (!::isdigit(*p)) return false;	// Must start with a digit
	++p;
	if (*p == ':') return true;
	if (!::isdigit(*p)) return false;
	++p;
	return *p == ':';
}

uint32_t DateTime::ToFATTime() const
{
	return ((static_cast<uint32_t>(year) - 1980) << 25) | (static_cast<uint32_t>(month) << 21) | (static_cast<uint32_t>(day) << 16) |
		(static_cast<uint32_t>(hour) << 11) | (static_cast<uint32_t>(min) << 5) | (static_cast<uint32_t>(sec) >> 1);
}

void DateTime::FromFATTime(uint32_t fattime)
{
	year	= static_cast<int16_t>(((fattime >> 25) & 0x7f) + 1980);
	month	= static_cast<int8_t>((fattime >> 21) & 0x0f);
	day		= static_cast<int8_t>((fattime >> 16) & 0x1f);
	hour	= static_cast<int8_t>((fattime >> 11) & 0x1f);
	min		= static_cast<int8_t>((fattime >> 5) & 0x3f);
	sec		= static_cast<int8_t>((fattime & 0x1f) << 1);
	msec	= 0;
}

}
