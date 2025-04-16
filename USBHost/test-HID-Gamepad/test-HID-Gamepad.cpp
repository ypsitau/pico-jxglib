#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	Printable::SetStandardOutput(Stdio::Instance);
	USBHost::Initialize();
	USBHost::GenericHID& gamePad = USBHost::GetGamePad();
	::printf("----\n");
	for (;;) {
		::printf("%d%d%d%d%d%d%d%d%d%d%d%d%d %x %02x %02x %02x %02x\n",
			gamePad.GetReportValue(0x00090001),	// X
			gamePad.GetReportValue(0x00090002),	// Y
			gamePad.GetReportValue(0x00090003),	// A
			gamePad.GetReportValue(0x00090004),	// B
			gamePad.GetReportValue(0x00090005),	// 5|LB
			gamePad.GetReportValue(0x00090006),	// 6|RB
			gamePad.GetReportValue(0x00090007),	// 7|LT
			gamePad.GetReportValue(0x00090008),	// 8|RT
			gamePad.GetReportValue(0x00090009),	// Left Stick
			gamePad.GetReportValue(0x0009000a),	// Right Stick
			gamePad.GetReportValue(0x0009000b),	// BACK
			gamePad.GetReportValue(0x0009000c),	// START 
			gamePad.GetReportValue(0x0009000d),	// GUIDE
			gamePad.GetReportValue(0x00010039),	// Hat Switch
			gamePad.GetReportValue(0x00010030),	// X
			gamePad.GetReportValue(0x00010031),	// Y
			gamePad.GetReportValue(0x00010032),	// Z
			gamePad.GetReportValue(0x00010035)	// Rz
		);
		Tickable::Sleep(100);
	}
}
