This time, let's talk about adding Terminal functionality to a TFT LCD. With scrolling, you can display a lot of information even on a small screen, and since you can read back the output later, it can also be useful as a data logger.

## About the Terminal Function

When it comes to displays, people tend to focus on graphics and GUIs, but in real applications, there are many situations where you want to display various text information. If you only have "display text at a specified coordinate on the screen," you can only output a limited amount of information. Terminal functionality, where you just send strings and the drawing position is updated automatically, and the screen scrolls when it overflows, is very convenient.

**pico-jxglib**'s Terminal provides the following natural and expected features:

1. Draw characters and strings while updating the drawing position
1. Automatically wrap to the next line when reaching the right edge
1. Scroll when reaching the bottom of the screen
1. Record output in a round line buffer and allow rollback
1. Read back the contents of the round line buffer

Especially, the last feature—reading back the round line buffer—can be combined with storage and communication features (to be implemented in **pico-jxglib**) to make an effective data logger.

Before we get to actual projects using Terminal, let's explain OLED devices, which are popular display devices for single-board microcontrollers alongside TFT LCDs.

## OLED Devices

OLED (organic EL display) devices often seen in embedded applications are controlled via I2C using the SSD1306 chip[^ssd1306]. The screen size is 0.96 inches, 128x64 pixels, white monochrome only, but since it's self-emissive, it's very clear. They're also attractive for their low price—about 500 yen each.

[^ssd1306]: Some devices have SPI as a control interface or different pixel counts, but the device described here is the most readily available.

![SSD1306.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/SSD1306.jpg)

Since the interface is I2C, you only need two signal lines. Communication speed is slower than SPI, but the data per pixel for SSD1306 is 1/16 that of RGB565 TFT LCD, and the total screen data is 128 * 64 / 8 = 1024 bytes. With I2C at 400kHz, refreshing the whole screen takes about 26 ms ([program](https://github.com/ypsitau/pico-jxglib/blob/main/Display/test-RefreshRate/test-RefreshRate.cpp)), which is fast enough for most uses.

Because it's monochrome and has few pixels, it's not suitable for complex graphics, but it's perfect for text output. For this Terminal, we'll support both TFT LCD and this OLED as output devices.

## Example Project

### Setting Up the Development Environment

If you haven't set up Visual Studio Code, Git tools, or the Pico SDK, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E9%96%8B%E7%99%BA%E7%92%B0%E5%A2%83).

You can get **pico-jxglib** by cloning the repository from GitHub:

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

```cmake title="CMakeLists.txt"
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

### Terminal on OLED SSD1306

The breadboard wiring image is as follows:

![circuit-ssd1306-Terminal.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/circuit-ssd1306-Terminal.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(termtest jxglib_Display_SSD1306)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp
::i2c_init(i2c0, 400 * 1000);
GPIO4.set_function_I2C0_SDA().pull_up();
GPIO5.set_function_I2C0_SCL().pull_up();
```

This initializes I2C and assigns pins to GPIO.

```cpp
Display::SSD1306 display(i2c0, 0x3c);
display.Initialize();
```

This initializes the OLED. Specify `0x3c` or `0x3d` for the I2C address.

### Terminal on TFT LCD ILI9341 (+ LVGL)

By specifying the drawing area when attaching a display to the Terminal, you can display multiple Terminals or share the display with other uses. Here, we'll combine LVGL (introduced in [the previous article](https://zenn.dev/ypsitau/articles/2025-02-04-lvgl)) with the Terminal.

The breadboard wiring image is as follows. The ILI9341 has a touchscreen, so LVGL buttons on the display act as tact switches.

![circuit-ili9341-touch.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/circuit-ili9341-touch.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(termtest jxglib_Display_ILI9341 jxglib_LVGL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_LVGL(termtest LV_FONT_MONTSERRAT_14)
```

Edit the source file as follows:

The display area for the `Terminal` is specified as:

```cpp
terminal.AttachDisplay(display, {0, 0, 240, 220});
```

And for the `LVGLAdapter`:

```cpp
lvglAdapter.AttachDisplay(display, {0, 220, 240, 100});
```

Each specifies its own drawing area.

### Creating Multiple Terminals

You can also create multiple `Terminal` instances ([program](https://github.com/ypsitau/pico-jxglib/blob/main/Terminal/test-Multi/test-Multi.cpp)).

![termtest-Multi.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/termtest-Multi.jpg)

Each can scroll and operate independently, making it convenient for displaying various information on a single screen.
