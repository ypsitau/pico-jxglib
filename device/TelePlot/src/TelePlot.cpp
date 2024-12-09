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
TelePlot::Telemetry::Telemetry() : Telemetry(PrintableDumb::Instance, "", Timestamp::ProgramTime, 1)
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

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int8_t* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%d", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint8_t* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%u", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int16_t* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%d", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint16_t* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%u", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int32_t* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%d", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint32_t* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%u", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const float* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%g", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const double* values, int len)
{
	char buff[64];
	GetPrintable().Printf(">%s:", name_);
	for (int i = 0; i < len; i++) {
		GetPrintable().Printf("%s%s:%g", (i == 0)? "" : ",",
				MakeTimeStampForMultiple(buff, sizeof(buff)), values[i]);
		cnt_++;
	}
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
	case Timestamp::Sequence:
		::snprintf(buff, len, ":%d", cnt_ * sequenceStep_);
		break;
	case Timestamp::ProgramTime:
		::snprintf(buff, len, ":%d", GetTimeStamp());
		break;
	case Timestamp::ReceptionTime:
		buff[0] = '\0';
		break;
	default:
		buff[0] = '\0';
		break;
	}
	return buff;
}

const char* TelePlot::Telemetry::MakeTimeStampForMultiple(char* buff, int len)
{
	switch (timestamp_) {
	case Timestamp::Sequence:
		::snprintf(buff, len, "%d", cnt_ * sequenceStep_);
		break;
	case Timestamp::ProgramTime:
		::snprintf(buff, len, "%d", GetTimeStamp());
		break;
	case Timestamp::ReceptionTime:
		::snprintf(buff, len, "%d", cnt_);
		break;
	default:
		buff[0] = '\0';
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
