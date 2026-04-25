# Stdio Input

LVGL operations are typically touchscreens, but keyboard input is also possible. Here, we explain how to simulate keyboard input from Stdio.

The TFT LCD uses ST7789. The breadboard wiring image is as follows:

![circuit-st7789-uart.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/circuit-st7789-uart.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(lvgltest jxglib_Display_ST7789 jxglib_LVGL lvgl_examples)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_LVGL(lvgltest LV_FONT_MONTSERRAT_14)
```

Edit the source file as follows:

```cpp title="lvgltest.cpp"
#include <stdio.h>
#include <examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate90);
    LVGL::Initialize();
    LVGL::Adapter lvglAdapter;
    lvglAdapter.EnableDoubleBuff().AttachDisplay(display).AttachKeyboard(Stdio::GetKeyboard());
    ::lv_example_keyboard_1();
    for (;;) {
        ::sleep_ms(5);
        ::lv_timer_handler();
    }
}
```

`PgUp` and `PgDn` move the focus. `Enter` clicks the focused widget.
