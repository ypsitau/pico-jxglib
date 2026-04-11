#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/USBMSC.h"

using namespace jxglib;

FAT::USBMSC fat("C");
USBHost::MSC& ShellCmd_USBHost_MSC_GetMSC() { return fat.GetMSC(); } // callback function for ShellCmd_USBHost_MSC

//USBHost::MSC msc;
//USBHost::MSC& ShellCmd_USBHost_MSC_GetMSC() { return msc; }

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
