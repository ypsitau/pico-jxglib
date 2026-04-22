#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/RTC/DS3231.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Prepare the Shell with Stdio
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    // Prepare the RTC
    ::i2c_init(i2c0, 400'000);
    GPIO16.set_function_I2C0_SDA().pull_up();
    GPIO17.set_function_I2C0_SCL().pull_up();
    RTC::DS3231 rtc(i2c0);    
    // Declare the flash drive with a name and size (must be a multiple of 4096)
    LFS::Flash driveLFS1("J:", 0x1010'0000, 0x0004'0000);  // 256kB
    LFS::Flash driveLFS2("*K:", 0x1014'0000, 0x0004'0000); // 256kB .. primary drive
    FAT::Flash driveFAT1("L:", 0x1018'0000, 0x0004'0000);  // 256kB
    FAT::Flash driveFAT2("M:", 0x101c'0000, 0x0004'0000);  // 256kB
    for (;;) {
        Tickable::Tick();
    }
}
