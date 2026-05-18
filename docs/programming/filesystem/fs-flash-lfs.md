# Flash File System Using LittleFS

In this page, we will learn how to use the LittleFS file system on the flash memory with `LFS::Flash` class.

## Link and Include

Add the following line to your `CMakeLists.txt`.

```cmake title="CMakeLists.txt"
target_link_libraries(your-project jxglib_LFS_Flash)
```

Add the following line to your source file.

```cpp title="Your Source File"
#include "jxglib/LFS/Flash.h"
```

## Declaring a File System

Creating a `LFS::Flash` instance declares that the specified region of the flash memory is reserved for the LittleFS file system.

!!! abstract ""

    ```cpp
    LFS::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP)
    ```

    - `driveName`: A string name for the drive, can contain any characters
    - `addrXIP`: The starting address of the flash memory region for the LittleFS file system
    - `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the LittleFS file system

The construction below allocates a region from the end of the flash memory.

!!! abstract ""

    ```cpp
    LFS::Flash(const char* driveName, uint32_t bytesXIP)
    ```

    - `driveName`: A string name for the drive, can contain any characters
    - `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the LittleFS file system

Note that the constructor is just a declaration of the drive and does not modify the flash memory. The file system is created only when `format` command is executed on the shell or `FS::Format()` is called in the program for the declared drive.

## Sample Program

In this sample program, we create a `LFS::Flash` instance on the flash memory. The instance is assigned a unique drive name and a specific region of the flash memory as follows:

| Drive Name | File System Type | Flash Memory Region           | Size   |
|------------|-----------------|-------------------------------|--------|
| LFS:         | LFS      | 0x1010'0000-0x1020'0000        | 1MB  |

### Build and Flash the Program

Create a new Pico SDK project named `fs-flash-lfs`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── fs-flash-lfs/
    ├── CMakeLists.txt
    ├── fs-flash-lfs.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/fs-flash-lfs/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `fs-flash-lfs.cpp` as follows:

```cpp title="fs-flash-lfs.cpp" linenums="1"
{% include "./sample/fs-flash-lfs/fs-flash-lfs.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect the board.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

A prompt will appear like this:

```text
LFS:?>
```

`LFS` is the driver name specified by the `LFS::Flash` instance in the program. The current directory is now `?`, indicating the drive is not formatted.

`ls-drive` shows the drive information:

```text
LFS:?>ls-drive -r
 Drive  Format        Total Remarks
 LFS:   none              0 LFS::Flash 0x10100000-0x10200000 1024kB
```

Executing `format` command with the drive name formats the drive:

```text
LFS:?>format lfs:
drive lfs: formatted in LittleFS
LFS:/>ls-drive -r
 Drive  Format          Total Remarks
 LFS:   LFS         1048576 LFS::Flash 0x10100000-0x10200000 1024kB
```

The prompt changes to `LFS:/>`, indicating the drive is now formatted and ready to use. You can create files and directories as usual:

```text
LFS:/>touch file1 file2 file3
LFS:/>dir
-a--- 2000-01-01 00:00:00      0 file1
-a--- 2000-01-01 00:00:00      0 file2
-a--- 2000-01-01 00:00:00      0 file3
```
