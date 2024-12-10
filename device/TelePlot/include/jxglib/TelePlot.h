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
	enum class HorzAxis {
		Sequence,
		ProgramTime,
		ReceptionTime,
	};
	class ValueFormatter {
	public:
		virtual const char* Next(char* buff, int len) = 0;
	};
	template<typename T> class ValueFormatter_T : public ValueFormatter {
	private:
		const char* format_;
		const T* pValue_;
	public:
		ValueFormatter_T(const char* format, const T* pValue) : format_{format}, pValue_{pValue} {}
	public:
		virtual const char* Next(char* buff, int len) override {
			::snprintf(buff, len, format_, *pValue_);
			pValue_++;
			return buff;
		}
	};
	template<typename T> class ValueFormatterScaled_T : public ValueFormatter {
	private:
		const T* pValue_;
		double scale_;
	public:
		ValueFormatterScaled_T(const T* pValue, double scale) : pValue_{pValue}, scale_{scale} {}
	public:
		virtual const char* Next(char* buff, int len) override {
			::snprintf(buff, len, "%g", scale_ * *pValue_);
			pValue_++;
			return buff;
		}
	};
	class Telemetry {
	private:
		Printable& printable_;
		char name_[32];
		HorzAxis horzAxis_;
		int cnt_;
		absolute_time_t absTimeStart_;	// used for HorzAxis::Device
		int sequenceStep_;				// used for HorzAxis::Sequence
	public:
		Telemetry();
		Telemetry(Printable& printable, const char* name, HorzAxis horzAxis, int sequenceStep);
	public:
		Telemetry& Reset() { cnt_ = 0; return *this; }
	public:
		Printable& GetPrintable() { return printable_; }
		template<typename T> Telemetry& Plot_T(const char* format, T value);
		Telemetry& Plot(int value);
		Telemetry& Plot(float value);
		Telemetry& Plot(double value);
		//template<typename T> Telemetry& Plot_T(const char* format, const T* values, int nValues);
		Telemetry& Plot(ValueFormatter& valueFormatter, int nValues);
		Telemetry& Plot(const int8_t* values, int nValues);
		Telemetry& Plot(const uint8_t* values, int nValues);
		Telemetry& Plot(const int16_t* values, int nValues);
		Telemetry& Plot(const uint16_t* values, int nValues);
		Telemetry& Plot(const int32_t* values, int nValues);
		Telemetry& Plot(const uint32_t* values, int nValues);
		Telemetry& Plot(const float* values, int nValues);
		Telemetry& Plot(const double* values, int nValues);
		Telemetry& Plot(const int8_t* values, int nValues, double scale);
		Telemetry& Plot(const uint8_t* values, int nValues, double scale);
		Telemetry& Plot(const int16_t* values, int nValues, double scale);
		Telemetry& Plot(const uint16_t* values, int nValues, double scale);
		Telemetry& Plot(const int32_t* values, int nValues, double scale);
		Telemetry& Plot(const uint32_t* values, int nValues, double scale);
		Telemetry& Plot(const float* values, int nValues, double scale);
		Telemetry& Plot(const double* values, int nValues, double scale);
		template<typename T> Telemetry& PlotXY_T(const char* format, T x, T y);
		Telemetry& PlotXY(int x, int y);
		Telemetry& PlotXY(float x, float y);
		Telemetry& PlotXY(double x, double y);
		Telemetry& PlotXY(const Point& pt) { return PlotXY(pt.x, pt.y); }
		Telemetry& PlotXY(const PointFloat& pt) { return PlotXY(pt.x, pt.y); }
		Telemetry& Text(const char* str);
		template<typename T> Telemetry& Plot_T(const char* format, int valueHorzAxis, T value);
		Telemetry& Plot(int valueHorzAxis, int value);
		Telemetry& Plot(int valueHorzAxis, float value);
		Telemetry& Plot(int valueHorzAxis, double value);
		template<typename T> Telemetry& PlotXY_T(const char* format, T x, T y, int valueHorzAxis);
		Telemetry& PlotXY(int x, int y, int valueHorzAxis);
		Telemetry& PlotXY(float x, float y, int valueHorzAxis);
		Telemetry& PlotXY(double x, double y, int valueHorzAxis);
		Telemetry& PlotXY(const Point& pt, int valueHorzAxis) { return PlotXY(pt.x, pt.y, valueHorzAxis); }
		Telemetry& PlotXY(const PointFloat& pt, int valueHorzAxis) { return PlotXY(pt.x, pt.y, valueHorzAxis); }
		Telemetry& Text(int valueHorzAxis, const char* str);
	public:
		const char* MakeValueHorzAxis(char* buff, int len);
		const char* MakeValueHorzAxisForMultiple(char* buff, int len);
		int GetTimestamp();
	};
private:
	Printable& printable_;
public:
	TelePlot(Printable& printable) : printable_{printable} {}
public:
	Printable& GetPrintable() { return printable_; }
	Telemetry Add(const char* name, HorzAxis horzAxis = HorzAxis::Sequence, int sequenceStep = 1) {
		return Telemetry(printable_, name, horzAxis, sequenceStep);
	}
};

}

#endif
