#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBHost/MSC.h"

using namespace jxglib;

USBHost::MSC msc;

USBHost::MSC& ShellCmd_USBHost_MSC_GetMSC() { return msc; }

int main()
{
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	USBHost::Initialize();
	while (true) Tickable::Tick();
}
