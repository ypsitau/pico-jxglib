# Stdio Input

LVGL operations are typically touchscreens, but keyboard input is also possible. Here, we explain how to simulate keyboard input from Stdio.

## Wiring

The TFT LCD uses ST7789. The breadboard wiring image is as follows:

![circuit-st7789-uart.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/circuit-st7789-uart.png)

### Building and Flashing the Program

Create a new Pico SDK project named `lvgl-stdio`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── lvgl-stdio/
    ├── CMakeLists.txt
    ├── lvgl-stdio.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/lvgl-stdio/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `lvgl-stdio.cpp` as follows:

```cpp title="lvgl-stdio.cpp" linenums="1"
{% include "./sample/lvgl-stdio/lvgl-stdio.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

`PgUp` and `PgDn` move the focus. `Enter` clicks the focused widget.
