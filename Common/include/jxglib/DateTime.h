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
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t msec;
public:
	constexpr DateTime() :
		year{2000}, month{1}, day{1}, hour{0}, minute{0}, second{0}, msec{0} {}
	constexpr DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint16_t msec) :
		year{year}, month{month}, day{day}, hour{hour}, minute{minute}, second{second}, msec{msec} {}
	constexpr DateTime(const DateTime& dt) :
		year{dt.year}, month{dt.month}, day{dt.day}, hour{dt.hour}, minute{dt.minute}, second{dt.second}, msec{dt.msec} {}
	constexpr DateTime& operator=(const DateTime& dt) {
		if (this != &dt) {
			year = dt.year;
			month = dt.month;
			day = dt.day;
			hour = dt.hour;
			minute = dt.minute;
			second = dt.second;
			msec = dt.msec;
		}
		return *this;
	}
};

}

#endif
