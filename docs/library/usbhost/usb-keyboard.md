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

### Operating the USB Keyboard

To use the USB host functionality in **pico-jxglib**, use the `USBHost` class.

Create a `USBHost::Keyboard` instance and use the following APIs to get keyboard information:

- `Keyboard::IsPressed()` — Checks if a specified key is pressed
- `Keyboard::ScanKeyCode()` — Returns the keycode of the pressed key
- `Keyboard::ScanKeyData()` — Returns the `KeyData` (keycode and character code) of the pressed key
- `Keyboard::GetKeyCode()` — While a key is pressed, repeats and returns the keycode; blocks if no key is pressed
- `Keyboard::GetKeyCodeNB()` — While a key is pressed, repeats and returns the keycode; returns `false` immediately if no key is pressed
- `Keyboard::GetKeyData()` — While a key is pressed, repeats and returns the `KeyData`; blocks if no key is pressed
- `Keyboard::GetKeyDataNB()` — While a key is pressed, repeats and returns the `KeyData`; returns `false` immediately if no key is pressed

Add the following lines to `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(usbhosttest jxglib_USBHost)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhosttest CFG_TUH_HID 3)
```

The `jxglib_configure_USBHost()` function generates the `tusb_config.h` header file needed for building tinyusb. The argument `CFG_TUH_HID` specifies the maximum number of HID interfaces to connect.

Below is an example using `Keyboard::IsPressed()`. The LED lights up when the specified key is pressed.

```cpp title="usbhosttest.cpp"
```

The `Tickable::Sleep()` function is specified in milliseconds. This function runs the tinyusb and **pico-jxglib** tasks.

Below is an example using `Keyboard::GetKeyCodeNB()`. The LED blinks in response to the repeated key presses.

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
    USBHost::Keyboard keyboard;
    for (;;) {
        uint8_t keyCode;
        bool rtn = keyboard.GetKeyCodeNB(&keyCode);
        GPIO18.put(rtn && keyCode == VK_V);
        GPIO19.put(rtn && keyCode == VK_C);
        GPIO20.put(rtn && keyCode == VK_X);
        GPIO21.put(rtn && keyCode == VK_Z);
        Tickable::Sleep(50);
    }
}

```
