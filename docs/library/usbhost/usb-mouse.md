## Example Project

Let's actually use a USB keyboard and mouse. Stdio can be used for detailed information, but since Stdio over USB can't be used in this setup, you can use LEDs on the Pico itself to confirm operation.

### Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86).

- **Name** ... Enter the project name. In this example, enter `usbhosttest`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... Select the port (UART or USB) to connect Stdio, but since USB is used for this program, you can't select it. Either select UART or leave both unchecked.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── usbhosttest/
    ├── CMakeLists.txt
    ├── usbhosttest.cpp
    └── ...
```

From here, edit `CMakeLists.txt` and the source file based on this project to create your program.

### Breadboard Wiring

Here is the breadboard wiring image:

![circuit-usbhost.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost.png)

When running the Pico as a USB device, you can power it from the USB connector, but when using it as a host, you need to supply 5V to VBUS (pin 40) from an external source. If you connect power to VSYS (pin 39), the diode for reverse current protection will prevent power from reaching USB VBUS, so be careful.

### Operating the USB Mouse

The USB mouse functionality is also implemented in the `USBHost` class.

Create a `USBHost::Mouse` instance and use the `Mouse::CaptureStatus()` function to get the mouse status. The information you get is as follows:

- Current position
- Movement amount
- Wheel operation amount
- Pan operation amount

Add the following lines to `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(usbhosttest jxglib_USBHost)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhosttest CFG_TUH_HID 3)
```

The mouse data example shows that the LED lights up when you move the mouse left or right, or when you click the left or right mouse button.

```cpp title="usbhosttest.cpp"
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"

using namespace jxglib;

int main()
{
    GPIO18.init().set_dir_OUT();
    GPIO19.init().set_dir_OUT();
    GPIO20.init().set_dir_OUT();
    GPIO21.init().set_dir_OUT();
    USBHost::Initialize();
    USBHost::Mouse mouse;
    for (;;) {
        Mouse::Status status = mouse.CaptureStatus();
        GPIO21.put(status.GetDeltaX() < 0);
        GPIO20.put(status.GetDeltaX() > 0);
        GPIO19.put(status.GetButtonLeft());
        GPIO18.put(status.GetButtonRight());
        Tickable::Sleep(50);
    }
}
```
