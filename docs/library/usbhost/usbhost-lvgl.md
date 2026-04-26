## Breadboard Wiring

The breadboard wiring image is as follows:

![circuit-usbhost-st7789.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost-st7789.png)

## Building and Flashing the Program

Create a new Pico SDK project named `usbhost-lvgl`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbhost-lvgl/
    ├── CMakeLists.txt
    ├── usbhost-lvgl.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/usbhost-lvgl/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `usbhost-lvgl.cpp` as follows:

```cpp title="usbhost-lvgl.cpp" linenums="1"
{% include "./sample/usbhost-lvgl/usbhost-lvgl.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

![lvgl-usbhid.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/lvgl-usbhid.jpg)

## About the Program

The `LVGL::Adapter` instance is used to connect the USB keyboard and mouse to LVGL. The `Tickable::Tick()` function runs the tinyusb, LVGL, and **pico-jxglib** tasks.
