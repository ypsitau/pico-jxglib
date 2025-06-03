//==============================================================================
// DateTime.cpp
//==============================================================================
#include "jxglib/DateTime.h"

namespace jxglib {

//------------------------------------------------------------------------------
// DateTime
//------------------------------------------------------------------------------
const DateTime DateTime::Empty;

DateTime& DateTime::operator=(const DateTime& dt)
{
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

int DateTime::Compare(const DateTime& dt1, const DateTime& dt2)
{
    return (dt1.year != dt2.year)? (dt1.year - dt2.year) :
        (dt1.month != dt2.month)? (dt1.month - dt2.month) :
        (dt1.day != dt2.day)? (dt1.day - dt2.day) :
        (dt1.hour != dt2.hour)? (dt1.hour - dt2.hour) :
        (dt1.minute != dt2.minute)? (dt1.minute - dt2.minute) :
        (dt1.second != dt2.second)? (dt1.second - dt2.second) :
        (dt1.msec - dt2.msec);
}

}
