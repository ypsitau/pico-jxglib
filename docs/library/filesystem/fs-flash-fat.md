# Flash File System Using FAT

## Build and Flash the Program

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
