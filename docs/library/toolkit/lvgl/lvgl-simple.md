# Simple Program

Connect an ILI9341 with a touchscreen and run a program using LVGL. For devices without a touchscreen, you can simulate keyboard operations via Stdio. For using USB keyboards and mice with LVGL, see the following article:

https://zenn.dev/ypsitau/articles/2025-04-02-usbhost-keyboard-mouse#lvgl-%E3%81%A8-usb-%E3%82%AD%E3%83%BC%E3%83%9C%E3%83%BC%E3%83%89%E3%83%BB%E3%83%9E%E3%82%A6%E3%82%B9

## Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86).

- **Name** ... Enter the project name. In this example, enter `lvgltest`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... Select the port (UART or USB) to connect Stdio.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── lvgltest/
    ├── CMakeLists.txt
    ├── lvgltest.cpp
    └── ...
```

From here, edit `CMakeLists.txt` and the source file based on this project to create your program.

## Building and Running the Sample Program

The breadboard wiring image is as follows:

![circuit-ili9341-touch.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/circuit-ili9341-touch.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(lvgltest jxglib_Display_ILI9341 jxglib_LVGL lvgl_examples)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_LVGL(lvgltest LV_FONT_MONTSERRAT_14)
```

The `jxglib_configure_LVGL()` function generates the `lv_conf.h` header file needed for building LVGL. List the macro variable names for the fonts you want to use as arguments. The first font specified becomes the default font.

Edit the source file as follows:

```cpp title="lvgltest.cpp"
#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::spi_init(spi0, 2 * 1000 * 1000);
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO2.set_function_SPI0_SCK();
    GPIO3.set_function_SPI0_TX();
    GPIO4.set_function_SPI0_RX();
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    Display::ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    Display::ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
    display.Initialize(Display::Dir::Rotate90);
    touchScreen.Initialize(display);
    //touchScreen.Calibrate(display);
    LVGL::Initialize();
    LVGL::Adapter lvglAdapter;
    lvglAdapter.EnableDoubleBuff().AttachDisplay(display).AttachTouchScreen(touchScreen);
    ::lv_example_anim_3();
    for (;;) {
        ::sleep_ms(5);
        ::lv_timer_handler();
    }
}
```

![lvgltest.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/lvgltest.jpg)

## Program Explanation

The program's processing content is explained as follows:

```cpp
::spi_init(spi0, 2 * 1000 * 1000);
::spi_init(spi1, 125 * 1000 * 1000);
```

Initialize SPI0 at 2MHz for the touchscreen and SPI1 at 125MHz for the TFT LCD.

```cpp
GPIO2.set_function_SPI0_SCK();
GPIO3.set_function_SPI0_TX();
GPIO4.set_function_SPI0_RX();
GPIO14.set_function_SPI1_SCK();
GPIO15.set_function_SPI1_TX();
```

Assign GPIOs to the SPI0 and SPI1 signal lines.

```cpp
Display::ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
Display::ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO6, IRQ: GPIO7});
display.Initialize(Display::Dir::Rotate90);
touchScreen.Initialize(display);
//touchScreen.Calibrate(display);
```

Assign SPI and GPIO to the TFT LCD and touchscreen parts, initializing them. The touchscreen's screen coordinates are mapped to preset values from the device's calibration, but the device-specific variations are still unknown. If the screen is too far off, call the `Calibrate()` function to calibrate.

```cpp
LVGL::Initialize();
```

Initialize LVGL.

```cpp
LVGL::Adapter lvglAdapter;
lvglAdapter.EnableDoubleBuff().AttachDisplay(display).AttachTouchScreen(touchScreen);
```

Use `LVGL::Adapter` to connect the TFT LCD and touchscreen to LVGL. `EnableDoubleBuff()` enables double buffering, which increases drawing speed using DMA. However, it consumes 2 times the memory.

```cpp
::lv_example_anim_3();
```

Call LVGL's sample program functions. Internally, widget creation and callback registration occur.

```cpp
for (;;) {
    ::sleep_ms(5);
    ::lv_timer_handler();
}
```

The main loop. `::lv_timer_handler()` performs LVGL processing.
