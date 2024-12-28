#include <stdio.h>
#include <math.h>
#include "jxglib/TelePlot.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	const int nPoints = 100;
	uint16_t values_sin[nPoints];
	for (int i = 0; i < nPoints; i++) values_sin[i] = ::sin(2 * M_PI * 3 * i / nPoints) * 0x1ff + 0x200;
	TelePlot(UART::Default).Add("Hello").Unit("Volt").Plot(values_sin, nPoints, 3.3 / 0x3ff);
}
