# Flash File System Using LittleFS

## Build and Flash the Program

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

```cmake title="CMakeLists.txt"
{% include "./sample/fs-flash-lfs/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `fs-flash-lfs.cpp` as follows:

```cpp title="fs-flash-lfs.cpp"
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
 Drive  Format           Total Remarks
 LFS:   unmounted            0 LFS::Flash 0x101c0000-0x10200000 256kB
```

Executing `format` command with the drive name formats the drive:

```text
LFS:?>format lfs:
drive lfs: formatted in LittleFS
LFS:/>
```

The prompt changes to `/`, indicating the drive is now formatted and ready to use. You can create files and directories as usual:

```text
LFS:/>touch file1 file2 file3
LFS:/>dir
-a--- 2000-01-01 00:00:00      0 file1
-a--- 2000-01-01 00:00:00      0 file2
-a--- 2000-01-01 00:00:00      0 file3
```

## Program Explanation

`LFS::Flash` instances are generated on the Pico board's flash memory, allowing you to treat it as a LittleFS file system. The constructor details are as follows:

- `LFS::Flash(const char* driveName, uint32_t bytesXIP)`
  `drivename`: A string name for the drive, can contain any characters
  `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the LittleFS file system

!!! note
    `LFS::Flash` instances are generated, but the file system is not created. You must run `FS::Format()` to format the drive.
