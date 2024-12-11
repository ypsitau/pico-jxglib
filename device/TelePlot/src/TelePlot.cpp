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
TelePlot::Telemetry::Telemetry() : Telemetry(PrintableDumb::Instance, "", HorzAxis::ProgramTime, 1)
{}

TelePlot::Telemetry::Telemetry(Printable& printable, const char* name, HorzAxis timestamp, int sequenceStep) :
	printable_{printable}, horzAxis_{timestamp}, cnt_{0}, sequenceStep_{sequenceStep}, clearFlag_(false)
{
	::strncpy(name_, name, sizeof(name_));
}


template<typename T> TelePlot::Telemetry& TelePlot::Telemetry::Plot_T(const char* format, T value)
{
	char buff[64];
	GetPrintable().Printf(">%s%s:", name_, MakeValueHorzAxis(buff, sizeof(buff)));
	GetPrintable().Printf(format, value);
	PutEndOfLine();
	cnt_++;
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int value)
{
	return Plot_T("%d", value);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(float value)
{
	return Plot_T("%g", value);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(double value)
{
	return Plot_T("%g", value);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(ValueFormatter& valueFormatter, int nValues)
{
	int iCol = 0;
	const int nCols = 200;
	char buff[64];
	for (int i = 0; i < nValues; i++) {
		if (iCol == 0) {
			GetPrintable().Printf(">%s:", name_);
		} else {
			GetPrintable().Printf(";");
		}
		GetPrintable().Printf("%s:", MakeValueHorzAxisForMultiple(buff, sizeof(buff)));
		GetPrintable().Print(valueFormatter.Next(buff, sizeof(buff)));
		cnt_++;
		iCol++;
		if (iCol == nCols) {
			PutEndOfLine();
			iCol = 0;
		}
	}
	if (iCol > 0) PutEndOfLine();
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int8_t* values, int nValues)
{
	ValueFormatter_T<int8_t> valueFormatter("%d", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint8_t* values, int nValues)
{
	ValueFormatter_T<uint8_t> valueFormatter("%u", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int16_t* values, int nValues)
{
	ValueFormatter_T<int16_t> valueFormatter("%d", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint16_t* values, int nValues)
{
	ValueFormatter_T<uint16_t> valueFormatter("%u", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int32_t* values, int nValues)
{
	ValueFormatter_T<int32_t> valueFormatter("%d", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint32_t* values, int nValues)
{
	ValueFormatter_T<uint32_t> valueFormatter("%u", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const float* values, int nValues)
{
	ValueFormatter_T<float> valueFormatter("%g", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const double* values, int nValues)
{
	ValueFormatter_T<double> valueFormatter("%g", values);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int8_t* values, int nValues, double scale)
{
	ValueFormatterScaled_T<int8_t> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint8_t* values, int nValues, double scale)
{
	ValueFormatterScaled_T<uint8_t> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int16_t* values, int nValues, double scale)
{
	ValueFormatterScaled_T<int16_t> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint16_t* values, int nValues, double scale)
{
	ValueFormatterScaled_T<uint16_t> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const int32_t* values, int nValues, double scale)
{
	ValueFormatterScaled_T<int32_t> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const uint32_t* values, int nValues, double scale)
{
	ValueFormatterScaled_T<uint32_t> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const float* values, int nValues, double scale)
{
	ValueFormatterScaled_T<float> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(const double* values, int nValues, double scale)
{
	ValueFormatterScaled_T<double> valueFormatter(values, scale);
	return Plot(valueFormatter, nValues);
}

template<typename T> TelePlot::Telemetry& TelePlot::Telemetry::PlotXY_T(const char* format, T x, T y)
{
	char buff[64];
	GetPrintable().Printf(">%s", name_);
	GetPrintable().Printf(format, x);
	GetPrintable().Printf(format, y);
	GetPrintable().Printf("%s|xy", MakeValueHorzAxis(buff, sizeof(buff)));
	PutEndOfLine();
	cnt_++;
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y)
{
	return PlotXY_T(":%d", x, y);
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y)
{
	return PlotXY_T(":%g", x, y);
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(double x, double y)
{
	return PlotXY_T(":%g", x, y);
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(const char* str)
{
	GetPrintable().Printf(">%s:%s|t\n", name_, str);
	return *this;
}

template<typename T> TelePlot::Telemetry& TelePlot::Telemetry::Plot_T(const char* format, int valueHorzAxis, T value)
{
	GetPrintable().Printf(">%s:%d:", name_, valueHorzAxis);
	GetPrintable().Printf(format, value);
	PutEndOfLine();
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int valueHorzAxis, int value)
{
	return Plot_T("%g", valueHorzAxis, value);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int valueHorzAxis, float value)
{
	return Plot_T("%g", valueHorzAxis, value);
}

TelePlot::Telemetry& TelePlot::Telemetry::Plot(int valueHorzAxis, double value)
{
	return Plot_T("%g", valueHorzAxis, value);
}

template<typename T> TelePlot::Telemetry& TelePlot::Telemetry::PlotXY_T(const char* format, T x, T y, int valueHorzAxis)
{
	GetPrintable().Printf(">%s", name_);
	GetPrintable().Printf(format, x);
	GetPrintable().Printf(format, y);
	GetPrintable().Printf(":%d|xy", valueHorzAxis);
	PutEndOfLine();
	return *this;
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(int x, int y, int valueHorzAxis)
{
	return PlotXY_T(":%d", x, y, valueHorzAxis);
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(float x, float y, int valueHorzAxis)
{
	return PlotXY_T(":%g", x, y, valueHorzAxis);
}

TelePlot::Telemetry& TelePlot::Telemetry::PlotXY(double x, double y, int valueHorzAxis)
{
	return PlotXY_T(":%g", x, y, valueHorzAxis);
}

TelePlot::Telemetry& TelePlot::Telemetry::Text(int valueHorzAxis, const char* str)
{
	GetPrintable().Printf(">%s:%d:%s|t\n", name_, valueHorzAxis, str);
	return *this;
}

void TelePlot::Telemetry::PutEndOfLine()
{
	if (clearFlag_) {
		GetPrintable().Printf("|clr\n");
		clearFlag_ = false;
	} else {
		GetPrintable().Printf("\n");
	}
}

const char* TelePlot::Telemetry::MakeValueHorzAxis(char* buff, int len)
{
	switch (horzAxis_) {
	case HorzAxis::Sequence:
		::snprintf(buff, len, ":%d", cnt_ * sequenceStep_);
		break;
	case HorzAxis::ProgramTime:
		::snprintf(buff, len, ":%d", GetTimestamp());
		break;
	case HorzAxis::ReceptionTime:
		buff[0] = '\0';
		break;
	default:
		buff[0] = '\0';
		break;
	}
	return buff;
}

const char* TelePlot::Telemetry::MakeValueHorzAxisForMultiple(char* buff, int len)
{
	switch (horzAxis_) {
	case HorzAxis::Sequence:
		::snprintf(buff, len, "%d", cnt_ * sequenceStep_);
		break;
	case HorzAxis::ProgramTime:
	case HorzAxis::ReceptionTime:
		::snprintf(buff, len, "%d", cnt_);
		break;
	default:
		buff[0] = '\0';
		break;
	}
	return buff;
}

int TelePlot::Telemetry::GetTimestamp()
{
	int timeStampMSec = 0;
	if (cnt_ == 0) {
		absTimeStart_ = ::get_absolute_time();
	} else {
		timeStampMSec = static_cast<int>(::absolute_time_diff_us(absTimeStart_, ::get_absolute_time()) / 1000);
	}
	return timeStampMSec;
}

}
