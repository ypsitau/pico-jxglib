#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost host;
	host.Initialize();
	::printf("----\n");
	for (;;) {
		USBHost::Mouse::Status status = USBHost::GetMouse().CaptureStatus();
		::printf("x:%-4d y:%-4d wheel:%-3d Pan:%-3d Left:%d Right:%d Middle:%d Backward:%d Forward:%d\n",
			status.GetDeltaX(), status.GetDeltaY(), status.GetDeltaWheel(), status.GetPan(),
			status.GetButtonLeft(), status.GetButtonRight(), status.GetButtonMiddle(),
			status.GetButtonBackward(), status.GetButtonForward());
		Tickable::Sleep(100);
	}
}
