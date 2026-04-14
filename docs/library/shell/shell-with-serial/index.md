
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
