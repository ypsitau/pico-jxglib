This page explains how to build a LittleFS or FAT file system on the Pico board's flash memory, and how to read, write, and otherwise operate files.

## About the **pico-jxglib** File System

In embedded applications, it is common to use flash memory as a file system. Libraries like [littlefs](https://github.com/littlefs-project/littlefs) and [FatFs](https://elm-chan.org/fsw/ff/) make it relatively easy to implement a file system, but writing your own storage device handler from scratch can be a bit of a hassle. **pico-jxglib** wraps these libraries so you can easily handle file systems from Pico SDK programs.

The **pico-jxglib** file system has the following features:

- **Unified Interface**
  Supports both LittleFS and FAT as backends, allowing you to operate on files with a common interface regardless of the file system type.
- **Support for Various Storage Devices**
  Supports not only the Pico board's flash memory, but also removable media such as SD cards and USB storage.
- **Shell Command Support**
  Provides shell commands for file system operations, so you can interactively operate the file system on the Pico board.

For details on using SD cards, USB storage, shell commands, and implementing timestamps with RTC, see the following articles:

▶️ [Connecting SD Cards and USB Storage to the Pico Board with pico-jxglib](https://zenn.dev/ypsitau/articles/2025-06-06-fs-media)
▶️ [Operating the File System with Shell Commands in pico-jxglib (Easy Input with Autocomplete and History)](https://zenn.dev/ypsitau/articles/2025-06-09-fs-shell)
▶️ [Connecting RTC to the Pico Board and Recording Timestamps in the File System with pico-jxglib](https://zenn.dev/ypsitau/articles/2025-06-22-rtc)

## Example Project

### Setting Up the Development Environment

If you haven't set up Visual Studio Code, Git tools, or the Pico SDK, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E9%96%8B%E7%99%BA%E7%92%B0%E5%A2%83).

Clone **pico-jxglib** from GitHub:

```bash
git clone https://github.com/ypsitau/pico-jxglib.git
cd pico-jxglib
git submodule update --init
```

:::message
**pico-jxglib** is updated almost daily. If you've already cloned it, run the following command in the `pico-jxglib` directory to get the latest version:

```bash
git pull
```
:::

### Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86).

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

### About Pico Board Flash Memory

The Pico board's flash memory has the following address ranges:

- Pico: 0x1000'0000 - 0x1020'0000 (2MB)
- Pico2: 0x1000'0000 - 0x1040'0000 (4MB)

The program is written from the head (0x1000'0000), so the remaining flash memory can be used as a file system. To check the range occupied by the program, use one of the following methods:

- **Check the map file**
  In the `build` directory of your project, a file like `something.elf.map` is generated. Search for `.flash_end` in this file. The value of this symbol is the end address of the flash memory occupied by the program.
- **Use picotool**
  Use `picotool` included in the Pico SDK to display information about the built ELF file. On your host PC, run the following command from the command prompt:

  [^picotool]: Windows の場合、`C:\Users\username\.pico-sdk\picotool\x.x.x\picotool` にパスを通す必要があります。

  ```bash
  picotool info build/something.elf
  ```

  You'll get output like:

  ```text
  File .\build\something.elf:
  
  Program Information
   name:          something
   version:       0.1
   features:      UART stdin / stdout
   binary start:  0x10000000
   binary end:    0x100049d4
  ```

  `binary end` is the end address of the flash memory occupied by the program.

- **Use pico-jxglib's shell**
  If the [pico-jxglib shell](https://zenn.dev/ypsitau/articles/2025-05-08-shell) is running on the Pico board, you can use the `about-me` command to get information about the running program. Connect to the Pico board's shell and run the following command:

  ```text
  > about-me
  ```

  This will display information similar to `picotool`, so you can check the `binary end` value.

The program's flash memory occupation is roughly 500kB for a 16-dot Japanese font (including first and second level). In subsequent examples, we'll reserve the first 1MB for the program and use the remaining (from address 0x1010'0000 onwards) for the file system.

### Drive Creation Methods

### LittleFS Drive Creation

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

:::message
`LFS::Flash` instances are generated, but the file system is not created. You must run `FS::Format()` to format the drive.
:::

#### FAT Drive Creation

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

:::message
`FAT::Flash` instances are generated, but the file system is not created. You must run `FS::Format()` to format the drive.
:::

#### Multiple Drive Creation

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

### File System API

#### File Writing

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

#### File Reading

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

#### Directory Information

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

### File and Directory Operations

**pico-jxglib** provides functions for file and directory operations. Here are some examples:

- `bool FS::CopyFile(const char* fileNameSrc, const char* fileNameDst);`
  Copies a file
- `bool FS::Move(const char* fileNameOld, const char* fileNameNew);`
  Renames or moves a file or directory (directory moving is not supported)
- `bool FS::RemoveFile(const char* fileName);`
  Deletes a file
- `bool FS::RemoveDir(const char* dirName);`
  Deletes a directory
- `bool FS::CreateDir(const char* dirName);`
  Creates a directory
- `bool FS::ChangeCurDir(const char* dirName);`
  Changes the current directory
- `bool FS::Format(const char* driveName);`
  Formats the file system
- `bool FS::Mount(const char* driveName);`
  Mounts the file system
- `bool FS::Unmount(const char* driveName);`
  Unmounts the file system
