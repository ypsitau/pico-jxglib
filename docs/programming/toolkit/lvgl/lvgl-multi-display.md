# Multiple Displays

Create multiple `LVGL::Adapter` instances and connect each to a TFT LCD or touchscreen to display LVGL GUIs on multiple screens.

In this example, we connect ILI9341 and ILI9488. The breadboard wiring image is as follows:

## Wiring

![circuit-ili9341-ili9488-touch.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/circuit-ili9341-ili9488-touch.png)

### Building and Flashing the Program

Create a new Pico SDK project named `lvgl-multi-display`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── lvgl-multi-display/
    ├── CMakeLists.txt
    ├── lvgl-multi-display.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/lvgl-multi-display/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `lvgl-multi-display.cpp` as follows:

```cpp title="lvgl-multi-display.cpp" linenums="1"
{% include "./sample/lvgl-multi-display/lvgl-multi-display.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

![lvgltest-multi.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/lvgltest-multi.jpg)

## Program Explanation

`LVGL::Adapter` instances have `SetDefault()` functions. After calling `SetDefault()`, all LVGL function calls are directed to that adapter. `SetPartialNum()` specifies how many times LVGL divides the screen. The more the number, the more divisions, and the smaller the buffer size. Usually, 10 divisions are set, but in this example, we have two LCDs and an additional ILI9488 with a large screen and 3 bytes per pixel, exceeding the Pico's RAM capacity. We increase the number of divisions to save memory.
