This page explains how to connect an RTC module to the Pico board and record timestamps in the file system. For file system operations using the shell, see the following article:

▶️ [Operating the File System with Shell Commands in pico-jxglib (Easy Input with Autocomplete and History)](https://zenn.dev/ypsitau/articles/2025-06-09-fs-shell)

## About RTC Modules

An RTC (Real-Time Clock) is a hardware device for keeping real-time date and time. The first-generation Pico (RP2040) has an RTC built into the chip, which at first seems convenient for keeping time without external hardware, but unfortunately, the time is lost when power is removed, so it's not very practical. In the successor Pico2 (RP2350), this module was removed, so it's not a candidate for real use.

A practical way to use an RTC is to connect an RTC module with a backup battery to the Pico board. This allows you to keep the time regardless of the Pico board's power state.

A commonly used RTC module in electronics is the DS3231, which connects via I2C. There is also a cheaper DS1307, but it can drift by several seconds per day, so the more accurate DS3231 is recommended.

The DS3231 module I purchased from Amazon looks like this:

![rtc-ds3231](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-22-rtc/rtc-ds3231.jpg)

It comes with a backup battery pre-installed and has a compact form factor. The signal names printed on the board may be hard to read, but `+` = `VCC`, `D` = `SDA`, `C` = `SCL`, and `-` = `GND`.

In this article, we connect this DS3231 module to the Pico board[^ds1307] and implement the following features using the RTC:

[^ds1307]: The DS1307 also shares I2C address and data format with the DS3231, so it should work similarly.

- Record timestamps in the file system
- Display the date and time in the shell prompt
- Create a clock application

### Example Project

### Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86).

- **Name** ... Enter the project name. In this example, enter `rtctest`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... Select the port (UART or USB) to connect to the terminal software.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── rtctest/
    ├── CMakeLists.txt
    ├── rtctest.cpp
    └── ...
```

From here, edit `CMakeLists.txt` and the source file based on this project to create your program.

### Recording Timestamps in the File System

Get the date and time from the RTC and record timestamps in the file system.

The breadboard wiring image is as follows:

![circuit-rtc](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-22-rtc/circuit-rtc.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(rtctest jxglib_RTC_DS3231 jxglib_LFS_Flash jxglib_FAT_Flash jxglib_Serial jxglib_ShellCmd_FS)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_FAT(rtctest FF_VOLUMES 1)
```

Edit the source file as follows:

```cpp title="rtctest.cpp"
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
    LFS::Flash driveA("A:",  0x1010'0000, 0x0004'0000); // Flash address and size 256kB
    LFS::Flash driveB("B:",  0x1014'0000, 0x0004'0000); // Flash address and size 256kB
    LFS::Flash driveC("*C:", 0x1018'0000, 0x0004'0000); // Flash address and size 256kB
    FAT::Flash driveD("D:",  0x101c'0000, 0x0004'0000); // Flash address and size 256kB
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    ::i2c_init(i2c0, 400'000);
    GPIO16.set_function_I2C0_SDA().pull_up();
    GPIO17.set_function_I2C0_SCL().pull_up();
    RTC::DS3231 rtc(i2c0);
    for (;;) {
        // :
        // any other jobs
        // :
        Tickable::Tick();
    }
}
```
