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
	//for (;;) Tickable::Sleep(1000);
#if 1
	for (;;) {
		USBHost::GamePad gamePad(USBHost::FindHID(0x00010005));
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
		if (gamePad.Get_ButtonX() && gamePad.GetHID().IsSendReady()) {
			::printf("check\n");
			uint8_t buff[8];
			buff[0] = 0xff;
			buff[1] = 0xff;
			buff[2] = 0xff;
			buff[3] = 0xff;
			buff[4] = 0xff;
			buff[5] = 0xff;
			buff[6] = 0xff;
			buff[7] = 0xff;
			gamePad.GetHID().SendReport(0x00, buff, 8);
		}
		Tickable::Sleep(300);
	}
#endif
}
