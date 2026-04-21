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

