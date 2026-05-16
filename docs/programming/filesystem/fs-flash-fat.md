# Flash File System Using FAT

## Link and Include

Add the following lines to your `CMakeLists.txt`.

```cmake title="CMakeLists.txt"
target_link_libraries(your-project jxglib_FAT_Flash)
jxglib_configure_FAT(your-project FF_VOLUMES 1)
```

Modify the number of `FF_VOLUMES` according to the number of FAT volumes you want to use.

Add the following line to your source file.

```cpp title="Your Source File"
#include "jxglib/FAT/Flash.h"
```

## API

!!! abstract ""

    ```cpp
    FAT::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP)
    ```

    - `driveName`: A string name for the drive, can contain any characters
    - `addrXIP`: The starting address of the flash memory region for the FAT file system
    - `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the FAT file system

    The constructor of `FAT::Flash` declares that the specified region of the flash memory is reserved for the FAT file system.

    Note that the constructor is just a declaration of the drive and does not modify the flash memory. The file system is created only when `format` command is executed on the shell or `FS::Format()` is called in the program for the declared drive.

## Sample Program

In this sample program, we create a `FAT::Flash` instance on the flash memory. The instance is assigned a unique drive name and a specific region of the flash memory as follows:

| Drive Name | File System Type | Flash Memory Region           | Size   |
|------------|-----------------|-------------------------------|--------|
| FAT:         | FAT      | 0x10100000-0x10200000        | 1MB  |

### Build and Flash the Program

Create a new Pico SDK project named `fs-flash-fat`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── fs-flash-fat/
    ├── CMakeLists.txt
    ├── fs-flash-fat.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/fs-flash-fat/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `fs-flash-fat.cpp` as follows:

```cpp title="fs-flash-fat.cpp" linenums="1"
{% include "./sample/fs-flash-fat/fs-flash-fat.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

### Running the Program

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
 Drive  Format        Total Remarks
 FAT:   none              0 FAT::Flash 0x10100000-0x10200000 1024kB
```

Executing `format` command with the drive name formats the drive:

```text
FAT:?>format fat:
drive fat: formatted in FAT12
FAT:/>ls-drive -r
 Drive  Format        Total Remarks
 FAT:   FAT12       1048576 FAT::Flash 0x10100000-0x10200000 1024kB
```

The prompt changes to `FAT:/>`, indicating the drive is now formatted and ready to use. You can create files and directories as usual:

```text
FAT:/>touch file1 file2 file3
FAT:/>dir
-a--- 2000-01-01 00:00:00      0 file1
-a--- 2000-01-01 00:00:00      0 file2
-a--- 2000-01-01 00:00:00      0 file3
```
