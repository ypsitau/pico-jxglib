#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Prepare the Shell with Stdio
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    // Declare the flash drive with a name and size (must be a multiple of 4096)
    LFS::Flash drive("LFS:", 0x0004'0000); // 256kB
    for (;;) {
        Tickable::Tick();
    }
}
