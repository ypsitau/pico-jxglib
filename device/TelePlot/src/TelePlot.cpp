//==============================================================================
// TelePlot.cpp
//==============================================================================
#include <stdio.h>
#include <string.h>
#include "jxglib/TelePlot.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TelePlot
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TelePlot::Telemetry
//------------------------------------------------------------------------------
TelePlot::Telemetry::Telemetry() : Telemetry(PrintableDumb::Instance, "", Timestamp::DeviceTime, 1)
{}

TelePlot::Telemetry::Telemetry(Printable& printable, const char* name, Timestamp timestamp, int sequenceStep) :
	printable_{printable}, timestamp_{timestamp}, cnt_{0}, sequenceStep_{sequenceStep}
{
	::strncpy(name_, name, sizeof(name_));
}


TelePlot::Telemetry& TelePlot::Telemetry::Plot(int value)
{
	char buff[64];
	GetPrintable().Printf(">%s%s:%d\n", name_, MakeTimeStamp(buff, sizeof(buff)), value);
	cnt_++;
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(float value)
{
	char buff[64];
	GetPrintable().Printf(">%s%s:%g\n", name_, MakeTimeStamp(buff, sizeof(buff)), value);
	cnt_++;
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y)
{
	char buff[64];
	GetPrintable().Printf(">%s:%d:%d%s|xy\n", name_, x, y, MakeTimeStamp(buff, sizeof(buff)));
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y)
{
	char buff[64];
	GetPrintable().Printf(">%s:%g:%g%s|xy\n", name_, x, y, MakeTimeStamp(buff, sizeof(buff)));
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(const char* str)
{
	GetPrintable().Printf(">%s:%s|t\n", name_, str);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int time, int value)
{
	GetPrintable().Printf(">%s:%d:%d\n", name_, time, value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int time, float value)
{
	GetPrintable().Printf(">%s:%d:%g\n", name_, time, value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y, int time)
{
	GetPrintable().Printf(">%s:%d:%d:%d|xy\n", name_, x, y, time);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y, int time)
{
	GetPrintable().Printf(">%s:%g:%g:%d|xy\n", name_, x, y, time);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(int time, const char* str)
{
	GetPrintable().Printf(">%s:%d:%s|t\n", name_, time, str);
	return *this;
}

const char* TelePlot::Telemetry::MakeTimeStamp(char* buff, int len)
{
	switch (timestamp_) {
	case Timestamp::ReceptionTime:
		buff[0] = '\0';
		break;
	case Timestamp::DeviceTime:
		::snprintf(buff, len, ":%d", GetTimeStamp());
		break;
	case Timestamp::Sequence:
		::snprintf(buff, len, ":%d", cnt_ * sequenceStep_);
		break;
	}
	return buff;
}

int TelePlot::Telemetry::GetTimeStamp()
{
	int time = 0;
	if (cnt_ == 0) {
		absTimeStart_ = ::get_absolute_time();
	} else {
		time = static_cast<int>(::absolute_time_diff_us(absTimeStart_, ::get_absolute_time()) / 1000);
	}
	return time;
}

}
