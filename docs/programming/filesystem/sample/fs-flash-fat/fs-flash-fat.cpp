#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Prepare the Shell with Stdio
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    // Declare the flash drive with a name and size (must be a multiple of 4096)
    FAT::Flash drive("FAT:", 0x1010'0000, 0x0010'0000); // 1MB
    for (;;) {
        Tickable::Tick();
    }
}
