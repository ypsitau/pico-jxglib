In this article, we will implement a shell function on the Pico board using the Terminal command input feature introduced in the previous article, ["pico-jxglib and Command Line Editing Features"](https://zenn.dev/ypsitau/articles/2025-04-06-cmdline-editor).

## Example Project

### Creating the Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](../../../development/pico-sdk/index.md).

- **Name** ... Enter the project name. In this example, enter `shell-test`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... If using a serial console for the terminal, select the port (UART or USB) to connect Stdio. If using a USB keyboard for the terminal, uncheck USB.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── shell-test/
    ├── CMakeLists.txt
    ├── shell-test.cpp
    └── ...
```

From here, edit `CMakeLists.txt` and the source file based on this project to create your program.


### Embedding the Shell

To embed the shell in your program, write the following code:

1. Create and initialize a `Terminal` instance (`Serial::Terminal` or `Display::Terminal`) to use with the shell
1. Connect the `Terminal` instance to the shell with the `Shell::AttachTerminal()` function
1. In the main loop, call `Tickable::Tick()` or `Tickable::Sleep()`

How you create the `Terminal` instance depends on what you use for the terminal. Here are some examples:

#### Using a Serial Console as the Terminal

This can be used in environments where Stdio is connected to a PC via UART or USB. Since the amount of code to write is small, you can easily embed the shell.

Add the following line to the end of `CMakeLists.txt`:

```cmake
target_link_libraries(shell-test jxglib_Serial jxglib_Shell jxglib_ShellCmd_Basic)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp title="shell-test.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    //-------------------------------------------------------------------------
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    terminal.Println("Shell on Serial Terminal");
    //-------------------------------------------------------------------------
    for (;;) {
        // any jobs
        Tickable::Tick();
    }
}
```

#### Using TFT LCD ST7789 and USB Keyboard as the Terminal

This can be used in environments where a TFT LCD ST7789 and USB keyboard are connected to the Pico board. Since you need to initialize the TFT LCD and set up SPI, there is more code to write, but it can run standalone on the Pico board.

The breadboard wiring image is as follows:

![circuit-usbhost-st7789.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-05-08-shell/circuit-usbhost-st7789.png)

Add the following line to the end of `CMakeLists.txt`. Also, make sure USB connection for Stdio is disabled (`pico_enable_stdio_usb(shell-test 0)`).


```cmake
target_link_libraries(shell-test jxglib_USBHost jxglib_Display_ST7789 jxglib_Shell jxglib_ShellCmd_Basic)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(shell-test CFG_TUH_HID 3)
```

Edit the source file as follows:

```cpp title="shell-test.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Shell.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    //-------------------------------------------------------------------------
    Display::Terminal terminal;
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    terminal.Initialize().AttachDisplay(display.Initialize(Display::Dir::Rotate90))
        .AttachKeyboard(keyboard.SetCapsLockAsCtrl()).SetFont(Font::shinonome16);
    Shell::AttachTerminal(terminal);
    terminal.Println("Shell on ST7789 TFT LCD");
    //-------------------------------------------------------------------------
    for (;;) {
        // any jobs
        Tickable::Tick();
    }
}
```

#### Using OLED SSD1306 and USB Keyboard as the Terminal

This can be used in environments where an OLED SSD1306 and USB keyboard are connected to the Pico board. Since you need to initialize the OLED and set up I2C, there is more code to write, but it can run standalone on the Pico board.

The breadboard wiring image is as follows:

![circuit-usbhost-ssd1306.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-05-08-shell/circuit-usbhost-ssd1306.png)

Add the following line to the end of `CMakeLists.txt`. Also, make sure USB connection for Stdio is disabled (`pico_enable_stdio_usb(shell-test 0)`).

```cmake
target_link_libraries(shell-test jxglib_USBHost jxglib_Display_SSD1306 jxglib_Shell jxglib_ShellCmd_Basic)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(shell-test CFG_TUH_HID 3)
```

Edit the source file as follows.

```cpp title="shell-test.cpp"
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/SSD1306.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Shell.h"
#include "jxglib/Font/shinonome12.h"
    GPIO5.set_function_I2C0_SCL().pull_up();
        .AttachKeyboard(keyboard.SetCapsLockAsCtrl()).SetFont(Font::shinonome12);
```

### Running Shell Commands

By linking the `jxglib_ShellCmd_Basic` library, you can use basic shell commands.

