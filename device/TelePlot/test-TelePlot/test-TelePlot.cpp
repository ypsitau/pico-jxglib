#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "jxglib/TelePlot.h"

using namespace jxglib;

TelePlot::Telemetry t1;

int main()
{
	int nPoints = 100;
	int nCycles = 3;
	const float pi = 3.14159;
	::stdio_init_all();
	TelePlot telePlot(UART::Default);
	do {
		TelePlot::Telemetry t = telePlot.Add("Sequnce", TelePlot::Timestamp::Sequence);
		for (int i = 0; i < nPoints; i++) t.Plot(::sin(2 * pi * nCycles * i / nPoints));
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("ProgramTime", TelePlot::Timestamp::ProgramTime);
		for (int i = 0; i < nPoints; i++) t.Plot(::sin(2 * pi * nCycles * i / nPoints));
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("ReceptionTime", TelePlot::Timestamp::ReceptionTime);
		for (int i = 0; i < nPoints; i++) t.Plot(::sin(2 * pi * nCycles * i / nPoints));
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("X-Y");
		for (int i = 0; i < nPoints; i++) {
			float radius = i * .1;
			t.PlotXY(radius * ::cos(2 * pi * nCycles * i / nPoints), radius * ::sin(2 * pi * nCycles * i / nPoints));
		}
	} while (0);
}
