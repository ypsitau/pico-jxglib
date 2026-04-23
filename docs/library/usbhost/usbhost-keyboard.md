## Wiring

Here is the breadboard wiring image:

![circuit-usbhost.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost.png)

When running the Pico as a USB device, you can power it from the USB connector, but when using it as a host, you need to supply 5V to VBUS (pin 40) from an external source. If you connect power to VSYS (pin 39), the diode for reverse current protection will prevent power from reaching USB VBUS, so be careful.

## Building and Flashing the Program

Create a new Pico SDK project named `usbhost-keyboard`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbhost-keyboard/
    ├── CMakeLists.txt
    ├── usbhost-keyboard.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "./sample/usbhost-keyboard/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `usbhost-keyboard.cpp` as follows:

```cpp title="usbhost-keyboard.cpp"
{% include "./sample/usbhost-keyboard/usbhost-keyboard.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-stdio.md" %}

A prompt will appear in the terminal like this:

```text
>
```

## About the Program

To use the USB host functionality in **pico-jxglib**, use the `USBHost` class.

Create a `USBHost::Keyboard` instance and use the following APIs to get keyboard information:

- `Keyboard::IsPressed()` — Checks if a specified key is pressed
- `Keyboard::ScanKeyCode()` — Returns the keycode of the pressed key
- `Keyboard::ScanKeyData()` — Returns the `KeyData` (keycode and character code) of the pressed key
- `Keyboard::GetKeyCode()` — While a key is pressed, repeats and returns the keycode; blocks if no key is pressed
- `Keyboard::GetKeyCodeNB()` — While a key is pressed, repeats and returns the keycode; returns `false` immediately if no key is pressed
- `Keyboard::GetKeyData()` — While a key is pressed, repeats and returns the `KeyData`; blocks if no key is pressed
- `Keyboard::GetKeyDataNB()` — While a key is pressed, repeats and returns the `KeyData`; returns `false` immediately if no key is pressed

The `jxglib_configure_USBHost()` function generates the `tusb_config.h` header file needed for building tinyusb. The argument `CFG_TUH_HID` specifies the maximum number of HID interfaces to connect.

Below is an example using `Keyboard::IsPressed()`. The LED lights up when the specified key is pressed.

The `Tickable::Sleep()` function is specified in milliseconds. This function runs the tinyusb and **pico-jxglib** tasks.

Below is an example using `Keyboard::GetKeyCodeNB()`. The LED blinks in response to the repeated key presses.
