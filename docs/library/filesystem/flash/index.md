# File System on Internal Flash Memory

Let's turn the Pico board's internal flash memory into a file system. This page explains how to build a LittleFS or FAT file system on the Pico board's flash memory.

## About Flash Memory

The Pico board's flash memory has the following address ranges:

- Pico: 0x1000'0000 - 0x1020'0000 (2MB)
- Pico2: 0x1000'0000 - 0x1040'0000 (4MB)

The program is written from the head (0x1000'0000), so the remaining flash memory can be used as a file system. To check the range occupied by the program, use one of the following methods:

**Check the map file**

:   In the `build` directory of your project, a file like `your-project.elf.map` is generated. It should contains a symbol named `.flash_end` like the following:

    ```text title="your-project.elf.map"
    .flash_end      0x10005770       0x14
    ```

    The value of this symbol is the end address of the flash memory occupied by the program.

**Use picotool**

:   picotool is a command-line tool included in the Pico SDK that can display information about the built ELF file. On Windows, you can find the executable file in `C:\Users\username\.pico-sdk\picotool\x.x.x\picotool`. Run the following command from the terminal:

    ```bash
    picotool info build/your-project.elf
    ```

    and you'll get output like:

    ```text
    File .\build\your-project.elf:
  
    Program Information
     name:          your-project
     version:       0.1
     features:      UART stdin / stdout
     binary start:  0x10000000
     binary end:    0x10005770
     target chip:   RP2350
     image type:    ARM Secure
    ```

    `binary end` is the end address of the flash memory occupied by the program.

**Use pico-jxglib's shell**
:    If the pico-jxglib shell is running on the Pico board, you can use the `about-me` command to get information about the running program. Connect to the Pico board's shell and run the following command:

    ```text
    > about-me
    Program Information
     name:              your-project
     version:           0.1
     binary start:      0x10000000
     binary end:        0x10005770
        :
        :
    ```

You can decide the starting address of the file system by adding margin to the end address of the program. For example, if the program occupies up to 0x10005770, you can set the file system to start from 0x10008000. It depends on your expectation on how much your program will grow in the future.

## Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](../../../development/pico-sdk/index.md). 

- **Name** ... Enter the project name. In this example, enter `fs-flash`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... Select the port (UART or USB) to connect Stdio.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── fs-flash/
    ├── CMakeLists.txt
    ├── fs-flash.cpp
    └── ...
```

From here, edit `CMakeLists.txt` and the source file based on this project to create your program.

## Creation of LittleFS Drive

[LittleFs](https://github.com/littlefs-project/littlefs) is a lightweight file system designed for embedded applications, suitable for flash memory with limited write cycles. **pico-jxglib** provides a class `LFS::Flash` to implement LittleFS on the Pico board's flash memory.

To include LittleFS in your project, add the following lines to the end of `CMakeLists.txt`:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("LFS:", 0x0004'0000); // 256kB
    // any job
}
```

`LFS::Flash` instances are generated on the Pico board's flash memory, allowing you to treat it as a LittleFS file system. The constructor details are as follows:

- `LFS::Flash(const char* driveName, uint32_t bytesXIP)`
  `drivename`: A string name for the drive, can contain any characters
  `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the LittleFS file system

!!! note
    `LFS::Flash` instances are generated, but the file system is not created. You must run `FS::Format()` to format the drive.

## Creation of FAT Drive

FAT is a widely used generic file system, common on SD cards and USB storage. **pico-jxglib** provides a class `FAT::Flash` to implement FAT on the Pico board's flash memory.

To include FAT in your project, add the following lines to the end of `CMakeLists.txt`:

```cmake
target_link_libraries(fs-flash jxglib_FAT_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_FAT(fs-flash FF_VOLUMES 1)
```

FatFs library configuration file `ffconf.h` is generated by running `jxglib_configure_FAT()`. `FF_VOLUMES` is the number of FAT file systems (**pico-jxglib** settings are the same as the number of drives). Here, we specify 1 to ensure one FAT drive.

Edit the source file as follows:

```cpp title="fs-flash.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    FAT::Flash drive("FAT:", 0x0004'0000); // 256kB
    // any job
}
```

`FAT::Flash` instances are generated on the Pico board's flash memory, allowing you to treat it as a FAT file system. The constructor details are as follows:

- `FAT::Flash(const char* driveName, uint32_t bytesXIP)`
  `drivename`: A string name for the drive, can contain any characters
  `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the FAT file system

!!! note
    `FAT::Flash` instances are generated, but the file system is not created. You must run `FS::Format()` to format the drive.

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

## File System API

## File Writing

An example of writing a file to a LittleFS file system:

`CMakeLists.txt`'s end should include:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive:", 0x0004'0000);  // 256kB
    if (!FS::Mount("Drive:") && !FS::Format(Stdio::Instance, "Drive:")) return 1;
    FS::File* pFile = FS::OpenFile("Drive:/test.txt", "w");
    if (pFile) {
        for (int i = 0; i < 10; ++i) {
            pFile->Printf("Line %d\n", i + 1);
        }
        pFile->Close();
        delete pFile;
    }
}
```

- `LFS::Flash drive("Drive", 0x0004'0000);  // 256kB`
  Creates a LittleFS file system drive named `Drive` with 256kB size
- `if (!FS::Mount("Drive:") && !FS::Format(Stdio::Instance, "Drive:")) return 1;`
  Checks if the drive is mounted. If not, formats the drive
- `File* pFile = FS::OpenFile("Drive:/test.txt", "w");`
  Opens a file for writing
- `pFile->Printf("Line %d\n", i + 1);`
  Writes a line to the file
- `pFile->Close();`
  Closes the file
- `delete pFile;`
  Deletes the file pointer

The same operations are possible with FAT file systems.

### File Reading

An example of reading a file from a LittleFS file system:

`CMakeLists.txt`'s end should include:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive:", 0x0004'0000);  // 256kB
    FS::File* pFile = FS::OpenFile("Drive:/test.txt", "r");
    if (pFile) {
        char line[256];
        while (pFile->ReadLine(line, sizeof(line)) > 0) {
            ::printf("%s\n", line);      
        }
        pFile->Close();
        delete pFile;
    }
}
```

- `File* pFile = FS::OpenFile("Drive:/test.txt", "r");`
  Opens a file for reading
- `pFile->ReadLine(line, sizeof(line))`
  Reads a line from the file

### Directory Information

An example of getting directory information from a LittleFS file system:

`CMakeLists.txt`'s end should include:

```cmake
target_link_libraries(fs-flash jxglib_LFS_Flash)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="fs-flash.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    LFS::Flash drive("Drive:", 0x0004'0000);  // 256kB
    FS::Dir* pDir = FS::OpenDir("Drive:/");
    if (pDir) {
        FS::FileInfo* pFileInfo;
        while (pFileInfo = pDir->Read()) {
            ::printf("%-16s%d\n", pFileInfo->GetName(), pFileInfo->GetSize());
            delete pFileInfo;
        }
        pDir->Close();
        delete pDir;
    }
}
```

- `FS::Dir* pDir = FS::OpenDir("Drive:/");`
  Opens a directory
- `pFileInfo = pDir->Read()`
  Reads file information into an FS::FileInfo instance
- `pDir->Close();`
  Closes the directory

## API Reference

### FS::CopyFile

```cpp
bool FS::CopyFile(const char* fileNameSrc, const char* fileNameDst);
```

Copies a file

### FS::Move

```cpp
bool FS::Move(const char* fileNameOld, const char* fileNameNew);
```

Renames or moves a file or directory (directory moving is not supported)

### FS::RemoveFile

```cpp
bool FS::RemoveFile(const char* fileName);
```

Deletes a file

### FS::RemoveDir

```cpp
bool FS::RemoveDir(const char* dirName);
```

Deletes a directory

### FS::CreateDir

```cpp
bool FS::CreateDir(const char* dirName);
```

Creates a directory

### FS::ChangeCurDir

```cpp
bool FS::ChangeCurDir(const char* dirName);
```

Changes the current directory

### FS::Format

```cpp
bool FS::Format(const char* driveName);
```

Formats the file system

### FS::Mount

```cpp
bool FS::Mount(const char* driveName);
```

Mounts the file system

### FS::Unmount

```cpp
bool FS::Unmount(const char* driveName);
```

Unmounts the file system

