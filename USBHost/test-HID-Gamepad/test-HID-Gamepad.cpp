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
	for (;;) {
		::printf("----\n");
		Tickable::Sleep(1000);
	}
	for (;;) {
		::printf("%d%d%d%d%d%d%d%d%d%d%d%d%d %x %02x %02x %02x %02x\n",
			gamePad.GetReportValue_ButtonX(),		// X
			gamePad.GetReportValue_ButtonY(),		// Y
			gamePad.GetReportValue_ButtonA(),		// A
			gamePad.GetReportValue_ButtonB(),		// B
			gamePad.GetReportValue_ButtonLB(),		// 5|LB
			gamePad.GetReportValue_ButtonRB(),		// 6|RB
			gamePad.GetReportValue_ButtonLT(),		// 7|LT
			gamePad.GetReportValue_ButtonRT(),		// 8|RT
			gamePad.GetReportValue_ButtonLStick(),	// Left Stick
			gamePad.GetReportValue_ButtonRStick(),	// Right Stick
			gamePad.GetReportValue_ButtonBACK(),	// BACK
			gamePad.GetReportValue_ButtonSTART(),	// START 
			gamePad.GetReportValue_ButtonGUIDE(),	// GUIDE
			gamePad.GetReportValue_HatSwitch(),		// Hat Switch
			gamePad.GetReportValue_LStickHorz(),	// X
			gamePad.GetReportValue_LStickVert(),	// Y
			gamePad.GetReportValue_RStickHorz(),	// Z
			gamePad.GetReportValue_RStickVert()		// Rz
		);
		Tickable::Sleep(100);
	}
}
