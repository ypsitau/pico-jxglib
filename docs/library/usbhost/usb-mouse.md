### Breadboard Wiring

Here is the breadboard wiring image:

![circuit-usbhost.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost.png)

When running the Pico as a USB device, you can power it from the USB connector, but when using it as a host, you need to supply 5V to VBUS (pin 40) from an external source. If you connect power to VSYS (pin 39), the diode for reverse current protection will prevent power from reaching USB VBUS, so be careful.

## Building and Flashing the Program

Create a new Pico SDK project named `usb-mouse`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usb-mouse/
    ├── CMakeLists.txt
    ├── usb-mouse.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "./sample/usb-mouse/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `usb-mouse.cpp` as follows:

```cpp title="usb-mouse.cpp"
{% include "./sample/usb-mouse/usb-mouse.cpp" %}
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

The USB mouse functionality is also implemented in the `USBHost` class.

Create a `USBHost::Mouse` instance and use the `Mouse::CaptureStatus()` function to get the mouse status. The information you get is as follows:

- Current position
- Movement amount
- Wheel operation amount
- Pan operation amount
