# Multiple Displays

Create multiple `LVGL::Adapter` instances and connect each to a TFT LCD or touchscreen to display LVGL GUIs on multiple screens.

In this example, we connect ILI9341 and ILI9488. The breadboard wiring image is as follows:

![circuit-ili9341-ili9488-touch.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/circuit-ili9341-ili9488-touch.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(lvgltest jxglib_Display_ILI9341 jxglib_Display_ILI9488 jxglib_LVGL lvgl_examples)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_LVGL(lvgltest LV_FONT_MONTSERRAT_14)
```

Edit the source file as follows:

```cpp title="lvgltest.cpp"
#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Display/ILI9488.h"
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
    Display::ILI9341 display1(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    Display::ILI9488 display2(spi1, 320, 480, {RST: GPIO18, DC: GPIO19, CS: GPIO20, BL: GPIO21});
    Display::ILI9341::TouchScreen touchScreen1(spi0, {CS: GPIO8, IRQ: GPIO9});
    Display::ILI9488::TouchScreen touchScreen2(spi0, {CS: GPIO16, IRQ: GPIO17});
    display1.Initialize(Display::Dir::Rotate90);
    display2.Initialize(Display::Dir::Rotate90);
    touchScreen1.Initialize(display1);
    touchScreen2.Initialize(display2);
    LVGL::Initialize();
    LVGL::Adapter lvglAdapter1;
    lvglAdapter1.AttachDisplay(display1).AttachTouchScreen(touchScreen1);
    LVGL::Adapter lvglAdapter2;
    lvglAdapter2.SetPartialNum(20).AttachDisplay(display2).AttachTouchScreen(touchScreen2);
    lvglAdapter1.SetDefault();
    ::lv_example_anim_3();
    lvglAdapter2.SetDefault();
    ::lv_example_keyboard_1();
    for (;;) {
        ::sleep_ms(5);
        ::lv_timer_handler();
    }
}
```

![lvgltest-multi.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/lvgltest-multi.jpg)

LVGL::Adapter instances have `SetDefault()` functions. After calling `SetDefault()`, all LVGL function calls are directed to that adapter. `SetPartialNum()` specifies how many times LVGL divides the screen. The more the number, the more divisions, and the smaller the buffer size. Usually, 10 divisions are set, but in this example, we have two LCDs and an additional ILI9488 with a large screen and 3 bytes per pixel, exceeding the Pico's RAM capacity. We increase the number of divisions to save memory.
