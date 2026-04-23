#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Initialize the USB Host and Mouse
    USBHost::Initialize();
    USBHost::Mouse mouse;
    for (;;) {
        Mouse::Status status = mouse.CaptureStatus();
        const Point& pt = status.GetPoint();
        ::printf("x:%-3d y:%-3d wheel:%-3d buttons:%08b\n", pt.x, pt.y, status.GetDeltaWheel(), status.GetButtons());
        Tickable::Sleep(100);
    }
}
