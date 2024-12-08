#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "jxglib/TelePlot.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	TelePlot telePlot(UART::Default);
	do {
		TelePlot::Telemetry t = telePlot.Add("Sequnce", TelePlot::Timestamp::Sequence);
		for (int i = 0; i < 10; i++) {
			t.Plot((float)i * i);
		}
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("DeviceTime", TelePlot::Timestamp::DeviceTime);
		for (int i = 0; i < 10; i++) {
			t.Plot((float)i * i);
		}
	} while (0);
	do {
		TelePlot::Telemetry t = telePlot.Add("ReceptionTime", TelePlot::Timestamp::ReceptionTime);
		for (int i = 0; i < 10; i++) {
			t.Plot((float)i * i);
		}
	} while (0);
	//t.PlotXY(3, 3);
	//t.PlotXY(2, 1);
	//t.PlotXY(10, 4);
#if 0
	::sleep_ms(1000);
	t.Text(0, "str1");
	::sleep_ms(1000);
	t.Text(1, "str2");
	::sleep_ms(1000);
	t.Text(2, "str3");
	::sleep_ms(1000);
	t.Text(3, "str4");
	::sleep_ms(1000);
	t.Text(4, "str5");
	::sleep_ms(1000);
	t.Text(5, "str6");
#endif
}
