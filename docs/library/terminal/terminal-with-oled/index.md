### Terminal on OLED SSD1306

The breadboard wiring image is as follows:

![circuit-ssd1306-Terminal.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/circuit-ssd1306-Terminal.png)

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
