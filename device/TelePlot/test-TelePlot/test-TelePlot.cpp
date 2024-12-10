#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "jxglib/TelePlot.h"

using namespace jxglib;

int main()
{
	const int nPoints = 100;
	int nCycles = 3;
	const float pi = 3.14159;
	::stdio_init_all();
	int8_t values_int8[nPoints];
	int16_t values_int16[nPoints];
	int32_t values_int32[nPoints];
	uint8_t values_uint8[nPoints];
	uint16_t values_uint16[nPoints];
	uint32_t values_uint32[nPoints];
	float values_float[nPoints];
	double values_double[nPoints];
	for (int i = 0; i < nPoints; i++) {
		double value = ::sin(2 * pi * nCycles * i / nPoints);
		values_int8[i] = static_cast<int8_t>(value * 0x7f);
		values_int16[i] = static_cast<int16_t>(value * 0x7fff);
		values_int32[i] = static_cast<int32_t>(value * 0x7fffffff);
		values_uint8[i] = static_cast<uint8_t>(value * 0x7f + 0x80);
		values_uint16[i] = static_cast<uint16_t>(value * 0x7fff + 0x8000);
		values_uint32[i] = static_cast<uint32_t>(value * 0x7fffffff + 0x80000000);
		values_float[i] = static_cast<float>(value);
		values_double[i] = static_cast<double>(value);
	}
	TelePlot telePlot(UART::Default);
	do {
		TelePlot::Telemetry t = telePlot.Add("Sequnce", TelePlot::HorzAxis::Sequence);
		for (int i = 0; i < nPoints; i++) t.Plot(values_double[i]);
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("ProgramTime", TelePlot::HorzAxis::ProgramTime);
		for (int i = 0; i < nPoints; i++) t.Plot(values_double[i]);
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("ReceptionTime", TelePlot::HorzAxis::ReceptionTime);
		for (int i = 0; i < nPoints; i++) t.Plot(values_double[i]);
	} while (0);
		do {
		TelePlot::Telemetry t = telePlot.Add("X-Y");
		for (int i = 0; i < nPoints; i++) {
			float radius = i * .1;
			t.PlotXY(radius * ::cos(2 * pi * nCycles * i / nPoints), radius * ::sin(2 * pi * nCycles * i / nPoints));
		}
	} while (0);
	telePlot.Add("Multiple (int8)", TelePlot::HorzAxis::Sequence).Plot(values_int8, count_of(values_int8));
	telePlot.Add("Multiple (int16)", TelePlot::HorzAxis::Sequence).Plot(values_int16, count_of(values_int16));
	telePlot.Add("Multiple (int32)", TelePlot::HorzAxis::Sequence).Plot(values_int32, count_of(values_int32));
	telePlot.Add("Multiple (uint8)", TelePlot::HorzAxis::Sequence).Plot(values_uint8, count_of(values_uint8));
	telePlot.Add("Multiple (uint16)", TelePlot::HorzAxis::Sequence).Plot(values_uint16, count_of(values_uint16));
	telePlot.Add("Multiple (uint32)", TelePlot::HorzAxis::Sequence).Plot(values_uint32, count_of(values_uint32));
	telePlot.Add("Multiple (float)", TelePlot::HorzAxis::Sequence).Plot(values_float, count_of(values_float));
	telePlot.Add("Multiple (double)", TelePlot::HorzAxis::Sequence).Plot(values_double, count_of(values_double));
}
