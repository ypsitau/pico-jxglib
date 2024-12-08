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
	enum class Timestamp {
		Sequence,
		DeviceTime,
		ReceptionTime,
	};
	class Telemetry {
	private:
		Printable& printable_;
		char name_[32];
		Timestamp timestamp_;
		int cnt_;
		absolute_time_t absTimeStart_;	// used for Timestamp::Device
		int sequenceStep_;				// used for Timestamp::Sequence
	public:
		Telemetry();
		Telemetry(Printable& printable, const char* name, Timestamp timestamp, int sequenceStep);
	public:
		Printable& GetPrintable() { return printable_; }
		Telemetry& Plot(int value);
		Telemetry& Plot(float value);
		Telemetry& PlotXY(int x, int y);
		Telemetry& PlotXY(float x, float y);
		Telemetry& PlotXY(const Point& pt) { return PlotXY(pt.x, pt.y); }
		Telemetry& PlotXY(const PointFloat& pt) { return PlotXY(pt.x, pt.y); }
		Telemetry& Text(const char* str);
		Telemetry& Plot(int time, int value);
		Telemetry& Plot(int time, float value);
		Telemetry& PlotXY(int x, int y, int time);
		Telemetry& PlotXY(float x, float y, int time);
		Telemetry& PlotXY(const Point& pt, int time) { return PlotXY(pt.x, pt.y, time); }
		Telemetry& PlotXY(const PointFloat& pt, int time) { return PlotXY(pt.x, pt.y, time); }
		Telemetry& Text(int time, const char* str);
	public:
		const char* MakeTimeStamp(char* buff, int len);
		int GetTimeStamp();
	};
private:
	Printable& printable_;
public:
	TelePlot(Printable& printable) : printable_{printable} {}
public:
	Printable& GetPrintable() { return printable_; }
	Telemetry Add(const char* name, Timestamp timestamp = Timestamp::Sequence, int sequenceStep = 1) {
		return Telemetry(printable_, name, timestamp, sequenceStep);
	}
};

}

#endif
