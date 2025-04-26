#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	USBHost::GamePad gamePad;
	for (;;) {
		if (gamePad.IsMounted()) {
			::printf("%d%d%d%d%d%d%d%d%d%d%d%d%d %x % 1.4f % 1.4f % 1.4f % 1.4f % 1.4f % 1.4f % 1.4f % 1.4f % 1.4f\n",
				gamePad.Get_Button0(),
				gamePad.Get_Button1(),
				gamePad.Get_Button2(),
				gamePad.Get_Button3(),
				gamePad.Get_Button4(),
				gamePad.Get_Button5(),
				gamePad.Get_Button6(),
				gamePad.Get_Button7(),
				gamePad.Get_Button8(),
				gamePad.Get_Button9(),
				gamePad.Get_Button10(),
				gamePad.Get_Button11(),
				gamePad.Get_Button12(),
				gamePad.Get_HatSwitch(),
				gamePad.Get_Axis0(),
				gamePad.Get_Axis1(),
				gamePad.Get_Axis2(),
				gamePad.Get_Axis3(),
				gamePad.Get_Axis4(),
				gamePad.Get_Axis5(),
				gamePad.Get_Axis6(),
				gamePad.Get_Axis7(),
				gamePad.Get_Axis8());
		}
		Tickable::Sleep(300);
	}
}
