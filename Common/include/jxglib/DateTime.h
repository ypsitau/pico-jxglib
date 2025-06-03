//==============================================================================
// jxglib/DateTime.h
//==============================================================================
#ifndef PICO_JXGLIB_DATETIME_H
#define PICO_JXGLIB_DATETIME_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// DateTime
//------------------------------------------------------------------------------
struct DateTime {
private:
	int16_t year;
	int8_t month;
	int8_t day;
	int8_t hour;
	int8_t minute;
	int8_t second;
	int16_t msec;
public:
	constexpr DateTime() :
		year{2000}, month{1}, day{1}, hour{0}, minute{0}, second{0}, msec{0} {}
	constexpr DateTime(int16_t year, int8_t month, int8_t day, int8_t hour, int8_t minute, int8_t second, int16_t msec) :
		year{year}, month{month}, day{day}, hour{hour}, minute{minute}, second{second}, msec{msec} {}
	constexpr DateTime(const DateTime& dt) :
		year{dt.year}, month{dt.month}, day{dt.day}, hour{dt.hour}, minute{dt.minute}, second{dt.second}, msec{dt.msec} {}
public:
	DateTime& operator=(const DateTime& dt);
	static int Compare(const DateTime& dt1, const DateTime& dt2);
};

}

#endif
