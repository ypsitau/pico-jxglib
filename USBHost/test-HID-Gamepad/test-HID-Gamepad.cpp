#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	Mouse& mouse = USBHost::GetMouse();
	::printf("----\n");
	//for (;;) {
	//	Mouse::Status status = mouse.CaptureStatus();
	//	::printf("x:%3d y:%3d dx:%-4d dy:%-4d wheel:%-3d Pan:%-3d Left:%d Middle:%d Right:%d Backward:%d Forward:%d\n",
	//		status.GetPoint().x, status.GetPoint().y,
	//		status.GetDeltaX(), status.GetDeltaY(), status.GetDeltaWheel(), status.GetPan(),
	//		status.GetButtonLeft(), status.GetButtonMiddle(), status.GetButtonRight(),
	//		status.GetButtonBackward(), status.GetButtonForward());
	//	Tickable::Sleep(100);
	//}
	for (;;) Tickable::Tick();
}
