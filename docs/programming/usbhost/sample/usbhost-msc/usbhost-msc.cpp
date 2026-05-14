#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBHost/MSC.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
    // Initialize the USB Host and MSC
    USBHost::Initialize();
    USBHost::MSC msc;
    for (;;) {
        Tickable::Tick();
    }
}
