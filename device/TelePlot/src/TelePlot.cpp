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
TelePlot::Telemetry::Telemetry(TelePlot& telePlot, const char* name) : telePlot_{telePlot}, firstFlag_{true}
{
	::strncpy(name_, name, sizeof(name_));
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int value)
{
	//GetPrintable().Printf(">%s:%d\n", name_, value);
	GetPrintable().Printf(">%s:%lld:%d\n", name_, GetTimeStamp(), value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(float value)
{
	//GetPrintable().Printf(">%s:%f\n", name_, value);
	GetPrintable().Printf(">%s:%lld:%f\n", name_, GetTimeStamp(), value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y)
{
	GetPrintable().Printf(">%s:%d:%d|xy\n", name_, x, y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y)
{
	GetPrintable().Printf(">%s:%f:%f|xy\n", name_, x, y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const Point& pt)
{
	GetPrintable().Printf(">%s:%d:%d|xy\n", name_, pt.x, pt.y);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const PointFloat& pt)
{
	GetPrintable().Printf(">%s:%f:%f|xy\n", name_, pt.x, pt.y);
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
	GetPrintable().Printf(">%s:%d:%f\n", name_, timeStamp, value);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y, int timeStamp)
{
	GetPrintable().Printf(">%s:%d:%d:%d|xy\n", name_, x, y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y, int timeStamp)
{
	GetPrintable().Printf(">%s:%f:%f:%d|xy\n", name_, x, y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const Point& pt, int timeStamp)
{
	GetPrintable().Printf(">%s:%d:%d:%d|xy\n", name_, pt.x, pt.y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(const PointFloat& pt, int timeStamp)
{
	GetPrintable().Printf(">%s:%f:%f:%d|xy\n", name_, pt.x, pt.y, timeStamp);
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(int timeStamp, const char* str)
{
	GetPrintable().Printf(">%s:%d:%s|t\n", name_, timeStamp, str);
	return *this;
}

int64_t TelePlot::Telemetry::GetTimeStamp()
{
	int64_t timeStamp = 0;
	if (firstFlag_) {
		absTimeStart_ = ::get_absolute_time();
		firstFlag_ = false;
	} else {
		timeStamp = ::absolute_time_diff_us(absTimeStart_, ::get_absolute_time()) / 1000;
	}
	return timeStamp;
}

}
