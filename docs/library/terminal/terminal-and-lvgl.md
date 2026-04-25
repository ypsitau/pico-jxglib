### Terminal on TFT LCD ILI9341 (+ LVGL)

By specifying the drawing area when attaching a display to the Terminal, you can display multiple Terminals or share the display with other uses. Here, we'll combine LVGL (introduced in [the previous article](https://zenn.dev/ypsitau/articles/2025-02-04-lvgl)) with the Terminal.

The breadboard wiring image is as follows. The ILI9341 has a touchscreen, so LVGL buttons on the display act as tact switches.

![circuit-ili9341-touch.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/circuit-ili9341-touch.png)

## Building and Flashing the Program

Create a new Pico SDK project named `terminal-and-lvgl`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── terminal-and-lvgl/
    ├── CMakeLists.txt
    ├── terminal-and-lvgl.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/terminal-and-lvgl/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `terminal-and-lvgl.cpp` as follows:

```cpp title="terminal-and-lvgl.cpp" linenums="1"
{% include "./sample/terminal-and-lvgl/terminal-and-lvgl.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}



Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(termtest jxglib_Display_ILI9341 jxglib_LVGL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_LVGL(termtest LV_FONT_MONTSERRAT_14)
```

Edit the source file as follows:

The display area for the `Terminal` is specified as:

```cpp
terminal.AttachDisplay(display, {0, 0, 240, 220});
```

And for the `LVGLAdapter`:

```cpp
lvglAdapter.AttachDisplay(display, {0, 220, 240, 100});
```

Each specifies its own drawing area.
