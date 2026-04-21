# Flash File System Using LittleFS

## Build and Flash the Program

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

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
{% include "sample/fs-flash-lfs/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `fs-flash-lfs.cpp` as follows:

```cpp title="fs-flash-lfs.cpp"
{% include "sample/fs-flash-lfs/fs-flash-lfs.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

The prompt below should appear:

```text
LFS:?>
```





## Program Explanation

`LFS::Flash` instances are generated on the Pico board's flash memory, allowing you to treat it as a LittleFS file system. The constructor details are as follows:

- `LFS::Flash(const char* driveName, uint32_t bytesXIP)`
  `drivename`: A string name for the drive, can contain any characters
  `bytesXIP`: The number of bytes to reserve from the end of the flash memory for the LittleFS file system

!!! note
    `LFS::Flash` instances are generated, but the file system is not created. You must run `FS::Format()` to format the drive.
