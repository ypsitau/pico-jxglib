# Multiple Flash File Systems

## Build and Flash the Program

Create a new Pico SDK project named `fs-flash-multi`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── fs-flash-multi/
    ├── CMakeLists.txt
    ├── fs-flash-multi.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "./sample/fs-flash-multi/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `fs-flash-multi.cpp` as follows:

```cpp title="fs-flash-multi.cpp"
{% include "./sample/fs-flash-multi/fs-flash-multi.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect the board.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

A prompt will appear like this:

```text
FAT:?>
```

`FAT` is the driver name specified by the `FAT::Flash` instance in the program. The current directory is now `?`, indicating the drive is not formatted.

`ls-drive` shows the drive information:

```text
FAT:?>ls-drive -r
 Drive  Format           Total Remarks
 FAT:   unmounted            0 FAT::Flash 0x101c0000-0x10200000 256kB
```

Executing `format` command with the drive name formats the drive:

```text
FAT:?>format fat:
drive fat: formatted in FAT12
FAT:/>
```

The prompt changes to `/`, indicating the drive is now formatted and ready to use. You can create files and directories as usual:

```text
FAT:/>touch file1 file2 file3
FAT:/>dir
-a--- 2000-01-01 00:00:00      0 file1
-a--- 2000-01-01 00:00:00      0 file2
-a--- 2000-01-01 00:00:00      0 file3
```



## Creation of Multiple Drives

You can also implement multiple drives. For example, to have 2 LittleFS drives and 2 FAT drives on the Pico board, add the following lines to `CMakeLists.txt`:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash jxglib_FAT_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_FAT(fs-flash FF_VOLUMES 2)
```

`jxglib_configure_FAT()`'s argument `FF_VOLUMES` is 2, ensuring 2 FAT drives.

Edit the source file as follows:

```cpp title="fs-flash.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash driveLFS1("LFS1:", 0x1010'0000, 0x0004'0000);  // 256kB
    LFS::Flash driveLFS2("LFS2:", 0x1014'0000, 0x0004'0000);  // 256kB
    FAT::Flash driveFAT1("FAT1:", 0x1018'0000, 0x0004'0000);  // 256kB
    FAT::Flash driveFAT2("FAT2:", 0x101c'0000, 0x0004'0000);  // 256kB
    // any job
}
```

Different address ranges are specified to create 4 file systems on the Pico board's flash memory. The constructor details are as follows:

- `LFS::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP)`
  `FAT::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP)`
  `driveName`: A string name for the drive, can contain any characters
  `addrXIP`: The starting address of the flash memory for the drive
  `bytesXIP`: The size of the flash memory for the drive

