### Terminal on OLED SSD1306

The breadboard wiring image is as follows:

![circuit-ssd1306-Terminal.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/circuit-ssd1306-Terminal.png)

## Building and Flashing the Program

Create a new Pico SDK project named `terminal-with-oled`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── terminal-with-oled/
    ├── CMakeLists.txt
    ├── terminal-with-oled.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/terminal-with-oled/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `terminal-with-oled.cpp` as follows:

```cpp title="terminal-with-oled.cpp" linenums="1"
{% include "./sample/terminal-with-oled/terminal-with-oled.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}



Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(termtest jxglib_Display_SSD1306)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

Edit the source file as follows:

```cpp
::i2c_init(i2c0, 400 * 1000);
GPIO4.set_function_I2C0_SDA().pull_up();
GPIO5.set_function_I2C0_SCL().pull_up();
```

This initializes I2C and assigns pins to GPIO.

```cpp
Display::SSD1306 display(i2c0, 0x3c);
display.Initialize();
```

This initializes the OLED. Specify `0x3c` or `0x3d` for the I2C address.
