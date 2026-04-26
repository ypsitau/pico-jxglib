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

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/fs-flash-multi/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `fs-flash-multi.cpp` as follows:

```cpp title="fs-flash-multi.cpp" linenums="1"
{% include "./sample/fs-flash-multi/fs-flash-multi.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect the board.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

A prompt will appear like this:

```text
K:?>
```

In the prompt, `K` is the driver name specified by the `LFS::Flash` instance in the program. The current directory is now `?`, indicating the drive is not formatted.

This drive is chosen as the current because it is specified as a primary drive with the drive name prefixed by an asterisk (`*`) in the constructor.

`ls-drive` shows the drive information:

```text
K:?>ls-drive -r
 Drive  Format           Total Remarks
 J:     unmounted            0 LFS::Flash 0x10100000-0x10140000 256kB
*K:     unmounted            0 LFS::Flash 0x10140000-0x10180000 256kB
 L:     unmounted            0 FAT::Flash 0x10180000-0x101c0000 256kB
 M:     unmounted            0 FAT::Flash 0x101c0000-0x10200000 256kB
```

Executing `format` command with the drive name formats the drive. 

```text
K:?>format J: K: L: M:
drive J: formatted in LittleFS
drive K: formatted in LittleFS
drive L: formatted in FAT12
drive M: formatted in FAT12
K:/>
```

The prompt changes to `/`, indicating the drive is now formatted and ready to use. You can create files and directories as usual:

```text
K:/>touch file1 file2 file3
K:/>dir
-a--- 2000-01-01 00:00:00      0 file1
-a--- 2000-01-01 00:00:00      0 file2
-a--- 2000-01-01 00:00:00      0 file3
```
- `LFS::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP)`
  `FAT::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP)`
  `driveName`: A string name for the drive, can contain any characters
  `addrXIP`: The starting address of the flash memory for the drive
  `bytesXIP`: The size of the flash memory for the drive
