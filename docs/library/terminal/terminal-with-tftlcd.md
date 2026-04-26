# Terminal with TFT-LCD

## Building and Flashing the Program

Create a new Pico SDK project named `terminal-with-tftlcd`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── terminal-with-tftlcd/
    ├── CMakeLists.txt
    ├── terminal-with-tftlcd.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/terminal-with-tftlcd/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `terminal-with-tftlcd.cpp` as follows:

```cpp title="terminal-with-tftlcd.cpp" linenums="1"
{% include "./sample/terminal-with-tftlcd/terminal-with-tftlcd.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}


### Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](../../development/pico-sdk/index.md).

- **Name** ... Enter the project name. In this example, enter `termtest`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... Select the port (UART or USB) to connect Stdio.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── termtest/
    ├── CMakeLists.txt
    ├── termtest.cpp
    └── ...
```

From here, edit `CMakeLists.txt` and the source file based on this project to create your program.

### Terminal on TFT LCD ST7789

Let's implement Terminal functionality using the TFT LCD ST7789. For other devices, see ["pico-jxglib and TFT LCD"](https://zenn.dev/ypsitau/articles/2025-01-27-tft-lcd).

The breadboard wiring image is as follows. Tact switches are placed for operations like rollback.

![circuit-st7789-Terminal.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/circuit-st7789-Terminal.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
target_link_libraries(termtest jxglib_Display_ST7789)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

The first part of the program initializes SPI and the TFT LCD.

```cpp
terminal.AttachDisplay(display);
terminal.SetFont(Font::shinonome16).SetSpacingRatio(1., 1.2).ClearScreen();
```

Attach the display device (TFT LCD, OLED, etc.) to the `Terminal` instance with `AttachDisplay()`. Use `SetFont()` to set the font, and `SetSpacingRatio()` to set character and line spacing. Now you're ready to output text to the Terminal.

```cpp
terminal.Suppress();
terminal.Print(Text_Botchan);
terminal.Suppress(false);
for (int i = 1; i < 7; i++) {
    for (int j = 1; j < 13; j++) terminal.Printf("%3d", i * j);
    terminal.Println();
}
```

Use `Print()`, `Println()`, and `Printf()` to output strings to the `Terminal` instance. The `Suppress()` function temporarily suppresses actual drawing. `Suppress(false)` returns to normal drawing.

```cpp
if (!GPIO18.get()) terminal.RollUp();
if (!GPIO19.get()) terminal.RollDown();
```

Call `RollUp()` and `RollDown()` to scroll up and down.

```cpp
if (!GPIO20.get()) terminal.Dump.BytesPerRow(8).Ascii()(reinterpret_cast<const void*>(0x10000000), 64);
```

The `Dump()` function outputs a memory dump.

```cpp
if (!GPIO21.get()) terminal.CreateReader().WriteTo(stdout);
```

The `CreateReader()` function creates a `Stream` instance to read the contents of the line buffer. Here, `WriteTo()` is called on that `Stream` to output data to stdout.
