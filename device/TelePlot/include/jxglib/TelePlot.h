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
		ReceptionTime,
		DeviceTime,
		Sequence,
	};
	class Telemetry {
	private:
		TelePlot& telePlot_;
		char name_[32];
		Timestamp timestamp_;
		int cnt_;
		absolute_time_t absTimeStart_;	// used for Timestamp::Device
		int sequenceStep_;				// used for Timestamp::Sequence
	public:
		Telemetry(TelePlot& telePlot, const char* name, Timestamp timestamp, int sequenceStep);
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
		void PrintLineHeader();	
		int GetTimeStamp();
	};
private:
	Printable& printable_;
public:
	TelePlot(Printable& printable) : printable_{printable} {}
public:
	Printable& GetPrintable() { return printable_; }
	Telemetry Add(const char* name, Timestamp timestamp = Timestamp::DeviceTime, int sequenceStep = 1) {
		return Telemetry(*this, name, timestamp, sequenceStep);
	}
};

}

#endif
