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
	//for (;;) {
	//	::printf("----\n");
	//	Tickable::Sleep(1000);
	//}
	for (;;) {
		USBHost::GamePad& gamePad = USBHost::FindGamePad();
		::printf("%d%d%d%d%d%d%d%d%d%d%d%d%d %x %02x %02x %02x %02x\n",
			gamePad.GetReportValue_ButtonX(),
			gamePad.GetReportValue_ButtonY(),
			gamePad.GetReportValue_ButtonA(),
			gamePad.GetReportValue_ButtonB(),
			gamePad.GetReportValue_ButtonLB(),
			gamePad.GetReportValue_ButtonRB(),
			gamePad.GetReportValue_ButtonLT(),
			gamePad.GetReportValue_ButtonRT(),
			gamePad.GetReportValue_ButtonLStick(),
			gamePad.GetReportValue_ButtonRStick(),
			gamePad.GetReportValue_ButtonBACK(),
			gamePad.GetReportValue_ButtonSTART(), 
			gamePad.GetReportValue_ButtonGUIDE(),
			gamePad.GetReportValue_HatSwitch(),
			gamePad.GetReportValue_LStickHorz(),
			gamePad.GetReportValue_LStickVert(),
			gamePad.GetReportValue_RStickHorz(),
			gamePad.GetReportValue_RStickVert());
		Tickable::Sleep(300);
	}
}
