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
			gamePad.Get_ButtonX(),
			gamePad.Get_ButtonY(),
			gamePad.Get_ButtonA(),
			gamePad.Get_ButtonB(),
			gamePad.Get_ButtonLB(),
			gamePad.Get_ButtonRB(),
			gamePad.Get_ButtonLT(),
			gamePad.Get_ButtonRT(),
			gamePad.Get_ButtonLStick(),
			gamePad.Get_ButtonRStick(),
			gamePad.Get_ButtonBACK(),
			gamePad.Get_ButtonSTART(), 
			gamePad.Get_ButtonGUIDE(),
			gamePad.Get_HatSwitch(),
			gamePad.Get_LStickHorz(),
			gamePad.Get_LStickVert(),
			gamePad.Get_RStickHorz(),
			gamePad.Get_RStickVert());
		Tickable::Sleep(300);
	}
}
