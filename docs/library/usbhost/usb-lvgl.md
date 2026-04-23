## LVGL and USB Keyboard/Mouse

LVGL is a library for implementing GUIs on embedded devices. For details, see the following article.

https://zenn.dev/ypsitau/articles/2025-02-04-lvgl

LVGL's user interface often uses touch screens, but here we're connecting a USB keyboard and mouse. We'll use the ST7789 TFT LCD. For other devices, see ["pico-jxblib and TFT LCD"](https://zenn.dev/ypsitau/articles/2025-01-27-tft-lcd).

The breadboard wiring image is as follows:

![circuit-usbhost-st7789.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost-st7789.png)

Add the following lines to `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(usbhosttest jxglib_USBHost jxglib_Display_ST7789 jxglib_LVGL lvgl_examples)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhosttest CFG_TUH_HID 3)
jxglib_configure_LVGL(usbhosttest LV_FONT_MONTSERRAT_14)
```

The source code is as follows:

```cpp title="usbhosttest.cpp"
#include <lvgl/examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    USBHost::Mouse mouse;
    Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate90);
    LVGL::Initialize(5);
    LVGL::Adapter lvglAdapter;
    lvglAdapter.AttachDisplay(display)
        .AttachKeyboard(keyboard)
        .AttachMouse(mouse);
    ::lv_example_keyboard_1();
    for (;;) Tickable::Tick();
}
```

![lvgl-usbhid.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/lvgl-usbhid.jpg)

The `LVGL::Adapter` instance is used to connect the USB keyboard and mouse to LVGL. The `Tickable::Tick()` function runs the tinyusb, LVGL, and **pico-jxglib** tasks.
