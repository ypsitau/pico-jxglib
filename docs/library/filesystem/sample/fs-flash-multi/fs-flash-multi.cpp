#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Prepare the Shell with Stdio
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    // Declare the flash drive with a name and size (must be a multiple of 4096)
    LFS::Flash driveLFS1("LFS1:", 0x1010'0000, 0x0004'0000);  // 256kB
    LFS::Flash driveLFS2("LFS2:", 0x1014'0000, 0x0004'0000);  // 256kB
    FAT::Flash driveFAT1("FAT1:", 0x1018'0000, 0x0004'0000);  // 256kB
    FAT::Flash driveFAT2("FAT2:", 0x101c'0000, 0x0004'0000);  // 256kB
    for (;;) {
        Tickable::Tick();
    }
}
