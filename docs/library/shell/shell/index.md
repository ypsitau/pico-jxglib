
In this article, we will implement a shell function on the Pico board using the Terminal command input feature introduced in the previous article, ["pico-jxglib and Command Line Editing Features"](https://zenn.dev/ypsitau/articles/2025-04-06-cmdline-editor).

## About the Shell Function

In embedded board development, you often repeat the cycle of editing program code, writing to the board, and running it. The quickest way to change program behavior is to modify the code directly. However, if you have many parameters to change or want to tweak behavior step by step while observing results, this process can become cumbersome. In such cases, having a shell running on the board that can accept commands makes experimentation and testing much smoother. Especially when creating apps that use file systems like FatFS or LittleFS, a shell is very useful for checking directory contents and more.

The **pico-jxglib** shell function has the following features:

- **Supports various input/output devices**  
    Flexible UI configuration is possible, working with UART/USB serial terminals, TFT LCD or OLED displays, USB keyboards, and more.

- **Easily integrated into existing projects**  
    Runs in the background. When waiting for commands, it consumes almost no resources, so it can be integrated without interfering with existing code.

- **Easy to create and embed commands**
    You can easily create commands. By simply linking the source or library that implements the command, it becomes available from the shell, making embedding and removal easy.

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

### Running Shell Commands

By linking the `jxglib_ShellCmd_Basic` library, you can use basic shell commands.

#### `help` command

Displays a list of available commands.

```text
>help
about-me        prints information about this own program
about-platform  prints information about the platform

help            prints help strings for available commands
prompt          changes the command line prompt
The first argument is the start address, and the second argument is the number of bytes to display. To specify a hexadecimal value, prefix it with `0x`.
```

#### `about-me` command

Displays information about the currently running program (build info, pin layout info embedded with the `bi_decl()` macro, and memory map). The output format is similar to picotool.

```text
>about-me
Program Information
 name:              shell-test
 version:           0.1
 feature:           UART stdin / stdout
 binary start:      0x10000000
 binary end:        0x1000e960

Fixed Pin Information
 0:                 UART0 TX
 1:                 UART0 RX

Build Information
 sdk version:       2.1.1
 pico_board:        pico
 boot2_name:        boot2_w25q080
 build date:        May 13 2025
 build attributes:  Debug

Memory Map
 flash binary:      0x10000000-0x1000e960   59744
 ram vector table:  0x20000000-0x200000c0     192
 data:              0x200000c0-0x20000968    2216
 bss:               0x20000968-0x20001aa4    4412
 heap:              0x20001aa4-0x20040000  255324
 stack:             0x20040000-0x20042000    8192
```

#### `about-platform` command

Displays platform information for the Pico board.

```text
>about-platform
RP2350 (ARM) 150 MHz
Flash  0x10000000-0x10400000 4194304
SRAM   0x20000000-0x20082000  532480
```

#### `d` command

Outputs a dump image of memory or files. If run with no arguments, it displays memory contents from address 0x00000000.

```text
>d
00000000  00 1F 04 20 EB 00 00 00 35 00 00 00 31 00 00 00
00000010  4D 75 01 03 7A 00 C4 00 1D 00 00 00 00 23 02 88
00000020  9A 42 03 D0 43 88 04 30 91 42 F7 D1 18 1C 70 47
00000030  30 BF FD E7 F4 46 00 F0 05 F8 A7 48 00 21 01 60
```

If you run it again with no arguments, it displays the next block of memory.

```text
>d
00000040  41 60 E7 46 A5 48 00 21 C9 43 01 60 41 60 70 47
00000050  CA 9B 0D 5B F9 1D 00 00 28 43 29 20 32 30 32 30
00000060  20 52 61 73 70 62 65 72 72 79 20 50 69 20 54 72
00000070  61 64 69 6E 67 20 4C 74 64 00 50 33 09 03 52 33
```

The first argument is the start address, and the second argument is the number of bytes to display. To specify a hexadecimal value, prefix it with `0x`.

### Creating Shell Commands

```text
>d 0x10000000
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
```

```text
>d 0x10000000 128
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
10000040  34 F8 19 6E 01 21 19 66 00 20 18 66 1A 66 00 F0
10000050  2C F8 19 6E 19 6E 19 6E 05 20 00 F0 2F F8 01 21
10000060  08 42 F9 D1 00 21 99 60 1B 49 19 60 00 21 59 60
10000070  1A 49 1B 48 01 60 01 21 99 60 EB 21 19 66 A0 21
```

If you specify a string other than a number as an argument, it is interpreted as a file name and the file contents are dumped. This feature is enabled when a file system is mounted.

### Creating Shell Commands

To create a shell command, use the `ShellCmd` macro. The macro format is as follows:

```cpp
ShellCmd(name, "comment")
{
    // any code
    return Result::Success;
}
```

The following variables are passed to the command program:

- `Printable& tout` ... `Printable` instance. Used to display results
- `int argc` ... Number of arguments
- `char** argv` ... Argument strings. `argv[0]` contains the command name itself

Return `0` if there is no error, or `1` if an error occurs.

You do **not** need to register commands. When you create a command with the `ShellCmd` macro, it is automatically registered with the shell. With this mechanism, you can add commands simply by linking the source file that implements the command to the main program.

Below is a sample program that displays the contents of the arguments passed. If you add this code to `shell-test.cpp` above, or write it in a separate source file and add that file to `add_executable()`, you can use it as a command.

```cpp
ShellCmd(argtest, "tests command line arguments")
{
    for (int i = 0; i < argc; i++) {
        tout.Printf("argv[%d] \"%s\"\n", i, argv[i]);
    }
    return Result::Success;
}
```
