//==============================================================================
// TelePlot.cpp
//==============================================================================
#include <string.h>
#include "jxglib/TelePlot.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TelePlot
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TelePlot::Telemetry
//------------------------------------------------------------------------------
TelePlot::Telemetry::Telemetry(TelePlot& telePlot, const char* name, Timestamp timestamp, int sequenceStep) :
	telePlot_{telePlot}, timestamp_{timestamp}, cnt_{0}, sequenceStep_{sequenceStep}
{
	::strncpy(name_, name, sizeof(name_));
}


TelePlot::Telemetry& TelePlot::Telemetry::Plot(int value)
{
	PrintLineHeader();
	GetPrintable().Printf("%d\n", value);
	cnt_++;
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(float value)
{
	PrintLineHeader();
	GetPrintable().Printf("%g\n", value);
	cnt_++;
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y)
{
	GetPrintable().Printf(">%s:%d:%d|xy\n", name_, x, y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y)
{
	GetPrintable().Printf(">%s:%g:%g|xy\n", name_, x, y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const Point& pt)
{
	GetPrintable().Printf(">%s:%d:%d|xy\n", name_, pt.x, pt.y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const PointFloat& pt)
{
	GetPrintable().Printf(">%s:%g:%g|xy\n", name_, pt.x, pt.y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(const char* str)
{
	GetPrintable().Printf(">%s:%s|t\n", name_, str);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int timeStamp, int value)
{
	GetPrintable().Printf(">%s:%d:%d\n", name_, timeStamp, value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int timeStamp, float value)
{
	GetPrintable().Printf(">%s:%d:%g\n", name_, timeStamp, value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y, int timeStamp)
{
	GetPrintable().Printf(">%s:%d:%d:%d|xy\n", name_, x, y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y, int timeStamp)
{
	GetPrintable().Printf(">%s:%g:%g:%d|xy\n", name_, x, y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const Point& pt, int timeStamp)
{
	GetPrintable().Printf(">%s:%d:%d:%d|xy\n", name_, pt.x, pt.y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const PointFloat& pt, int timeStamp)
{
	GetPrintable().Printf(">%s:%g:%g:%d|xy\n", name_, pt.x, pt.y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(int timeStamp, const char* str)
{
	GetPrintable().Printf(">%s:%d:%s|t\n", name_, timeStamp, str);
	return *this;
}

void TelePlot::Telemetry::PrintLineHeader()
{
	switch (timestamp_) {
	case Timestamp::ReceptionTime:
		GetPrintable().Printf(">%s:", name_);
		break;
	case Timestamp::DeviceTime:
		GetPrintable().Printf(">%s:%d:", name_, GetTimeStamp());
		break;
	case Timestamp::Sequence:
		GetPrintable().Printf(">%s:%d:", name_, cnt_ * sequenceStep_);
		break;
	}
}

int TelePlot::Telemetry::GetTimeStamp()
{
	int timeStamp = 0;
	if (cnt_ == 0) {
		absTimeStart_ = ::get_absolute_time();
	} else {
		timeStamp = static_cast<int>(::absolute_time_diff_us(absTimeStart_, ::get_absolute_time()) / 1000);
	}
	return timeStamp;
}

}
