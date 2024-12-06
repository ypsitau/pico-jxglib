//==============================================================================
// jxglib/TelePlot.h
//==============================================================================
#ifndef PICO_JXGLIB_TELEPLOT_H
#define PICO_JXGLIB_TELEPLOT_H
#include "pico/stdlib.h"
#include "jxglib/Point.h"
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TelePlot
//------------------------------------------------------------------------------
class TelePlot {
public:
	class Telemetry {
	private:
		TelePlot& telePlot_;
		char name_[32];
		bool firstFlag_;
		absolute_time_t absTimeStart_;
	public:
		Telemetry(TelePlot& telePlot, const char* name);
	public:
		Printable& GetPrintable() { return telePlot_.GetPrintable(); }
		Telemetry& Plot(int value);
		Telemetry& Plot(float value);
		Telemetry& PlotXY(int x, int y);
		Telemetry& PlotXY(float x, float y);
		Telemetry& PlotXY(const Point& pt);
		Telemetry& PlotXY(const PointFloat& pt);
		Telemetry& Text(const char* str);
		Telemetry& Plot(int timeStamp, int value);
		Telemetry& Plot(int timeStamp, float value);
		Telemetry& PlotXY(int x, int y, int timeStamp);
		Telemetry& PlotXY(float x, float y, int timeStamp);
		Telemetry& PlotXY(const Point& pt, int timeStamp);
		Telemetry& PlotXY(const PointFloat& pt, int timeStamp);
		Telemetry& Text(int timeStamp, const char* str);
	public:
		int64_t GetTimeStamp();
	};
private:
	Printable& printable_;
public:
	TelePlot(Printable& printable) : printable_{printable} {}
public:
	Printable& GetPrintable() { return printable_; }
	Telemetry Add(const char* name) { return Telemetry(*this, name); }
};

}

#endif
