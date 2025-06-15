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
public:
	int16_t year;
	int8_t month;
	int8_t day;
	int8_t hour;
	int8_t min;
	int8_t sec;
	int16_t msec;
public:
	static const int8_t daysInMonth[12];
	static const DateTime Empty;
public:
	constexpr DateTime() :
		year{2000}, month{1}, day{1}, hour{0}, min{0}, sec{0}, msec{0} {}
	constexpr DateTime(int16_t year, int8_t month, int8_t day, int8_t hour, int8_t min, int8_t sec, int16_t msec) :
		year{year}, month{month}, day{day}, hour{hour}, min{min}, sec{sec}, msec{msec} {}
	constexpr DateTime(const DateTime& dt) :
		year{dt.year}, month{dt.month}, day{dt.day}, hour{dt.hour}, min{dt.min}, sec{dt.sec}, msec{dt.msec} {}
public:
	int8_t CalcDayOfWeek() const;
	bool Parse(const char* str);
	bool ParseDate(const char* str, const char** endPtr = nullptr);
	bool ParseTime(const char* str);
public:
	uint64_t ToUnixTime() const;
	void FromUnixTime(uint64_t unixtime);
	uint32_t ToFATTime() const;
	void FromFATTime(uint32_t fattime);
public:
	DateTime& operator=(const DateTime& dt);
	static int Compare(const DateTime& dt1, const DateTime& dt2);
	static bool IsLeapYear(int16_t year) { return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); }
	static bool HasDateFormat(const char* str);
	static bool HasTimeFormat(const char* str);
};

}

#endif
