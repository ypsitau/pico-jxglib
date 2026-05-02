# Library APIs

## Development Environment

The library APIs of pico-jxglib work along with the Pico SDK. Below is a quick introduction to set up the development environment:

{% include-markdown "include/quick-setup-picosdk.md" %}


## How to Use pico-jxglib in Your Project

If you don't have a Pico SDK project yet, create a new one as described below:

{% include-markdown "include/create-open-project.md" %}

When you create a new project with the name `your-project`, the project directory will look like this:

```text
└── your-project/
    ├── CMakeLists.txt
    ├── your-project.cpp
    └── ...
```

Then, clone the pico-jxglib repository from GitHub as follows:

{% include-markdown "include/clone-repository.md" %}

But, wait. Where should we put the `pico-jxglib` directory? There are two ways of arranging the directories of your project and pico-jxglib: 

!!! abstract "Inside Style"
    `pico-jxglib` is placed inside `your-project` directory like this:
    ```text hl_lines="2"
    └── your-project/
        ├── pico-jxglib/
        ├── CMakeLists.txt
        ├── your-project.cpp
        └── ...
    ```

    Add the following command to your `CMakeLists.txt`:

    ```cmake
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-jxglib)
    ```

    This style is useful when you want to include pico-jxglib in your project. It specifically works well with Git's submodule feature, allowing you to manage pico-jxglib as part of your project repository.

!!! abstract "Outside Style"
    `pico-jxglib` is placed in the same directory as `your-project` like this:
    ```text hl_lines="1"
    ├── pico-jxglib/
    └── your-project/
        ├── CMakeLists.txt
        ├── your-project.cpp
        └── ...
    ```

    Add the following command to your `CMakeLists.txt`:

    ```cmake
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```

    Second argument `pico-jxglib` is required in `add_subdirectory()` because the added directory contains a reference to a parent directory. The string in the second argument is used as the output directory name for generated files. As long as it doesn't conflict with other directories in `build`, you can use any name.

    This style is useful when you want to share pico-jxglib across multiple projects.

## Sample Programs

Using the project created above, let's create actual programs that use pico-jxglib. There are two sample programs:

- ***Blinky Program*** ... The simplest program that blinks an LED connected to a GPIO.
- ***LABOPlatform Program*** ... A more complicated program that has the same functionality as pico-jxgLABO, including the interactive shell and built-in logic analyzer.

Modify the `CMakeLists.txt` and `your-project.cpp` files as described below. It is assumed that `pico-jxglib` is placed in the same directory as `your-project` (outside style).

=== "Blinky Program"

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt" linenums="1"
    {% include "./labo-platform/sample/your-project-blinky/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
    ```

    Edit `your-project.cpp` as follows:

    ```cpp title="your-project.cpp" linenums="1"
    {% include "./labo-platform/sample/your-project-blinky/your-project.cpp" %}
    ```

=== "LABOPlatform Program"

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt" linenums="1"
    {% include "./labo-platform/sample/your-project-LABOPlatform/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
    ```

    Edit `your-project.cpp` as follows:

    ```cpp title="your-project.cpp" linenums="1"
    {% include "./labo-platform/sample/your-project-LABOPlatform/your-project.cpp" %}
    ```

## Libraries

pico-jxglib's libraries are prefixed with `jxglib_`. Here is the list of libraries in pico-jxglib:

|Library Name | Description|
|--- | ---|
|jxglib_Common | Common utilities and definitions |
|jxglib_GPIOInfo | GPIO information utilities |
|jxglib_BinaryInfo | Binary information utilities |
|jxglib_ShellCmd_Basic | Basic shell commands |
|jxglib_ShellCmd_GPIO | GPIO shell commands |
|jxglib_ShellCmd_ADC | ADC shell commands |
|jxglib_Font | Font support |
|jxglib_Camera | Camera interface and functionalities |
|jxglib_Camera_OV7670 | OV7670 camera module support |
|jxglib_ShellCmd_Camera | Shell commands for camera control |
|jxglib_ShellCmd_Camera_OV7670 | Shell commands for OV7670 camera module |
|jxglib_Canvas | Canvas drawing functionalities |
|jxglib_KeyboardTest | Keyboard testing utilities |
|jxglib_LCD1602 | LCD1602 display support |
|jxglib_Device_MCP4726 | MCP4726 device support |
|jxglib_Device_SDCard | SD card device support |
|jxglib_Device_WS2812 | WS2812 device support |
|jxglib_ShellCmd_Device_WS2812 | Shell commands for WS2812 device |
|jxglib_DMA | DMA support |
|jxglib_Flash | Flash memory support |
|jxglib_RTC | RTC support |
|jxglib_RTC_DS3231 | DS3231 RTC support |
|jxglib_RTC_Pico | Pico RTC support |
|jxglib_ShellCmd_RTC | Shell commands for RTC |
|jxglib_ShellCmd_RTC_DS3231 | Shell commands for DS3231 RTC |
|jxglib_ShellCmd_Flash | Shell commands for flash memory |
|jxglib_TEK4010 | TEK4010 support |
|jxglib_TelePlot | TelePlot support |
|jxglib_ShellCmd_Device_SDCard | Shell commands for SD card device |
|jxglib_StepMotor | Step motor support |
|jxglib_Display | Display support |
|jxglib_ShellCmd_Display | Shell commands for display |
|jxglib_Display_SSD1306 | SSD1306 display support |
|jxglib_ShellCmd_Display_SSD1306 | Shell commands for SSD1306 display |
|jxglib_Drawable | Drawable support |
|jxglib_Display_TFT_LCD | TFT LCD display support |
|jxglib_Display_ST7735 | ST7735 display support |
|jxglib_Display_ST7789 | ST7789 display support |
|jxglib_Display_ILI9341 | ILI9341 display support |
|jxglib_Display_ILI9488 | ILI9488 display support |
|jxglib_Display_WS2812 | WS2812 display support |
|jxglib_Display_VideoTransmitter | Video transmitter display support |
|jxglib_VideoTransmitter | Video transmitter support |
|jxglib_ShellCmd_Display_TFT_LCD | Shell commands for TFT LCD display |
|jxglib_ShellCmd_Display_WS2812 | Shell commands for WS2812 display |
|jxglib_ShellCmd_Display_VideoTransmitter | Shell commands for video transmitter display |
|jxglib_DrawableTest | Drawable test support |
|jxglib_ShellCmd_Font | Shell commands for font |
|jxglib_FS | File system support |
|jxglib_FAT | FAT file system support |
|jxglib_FAT_SDCard | FAT file system support for SD cards |
|jxglib_FAT_USBMSC | FAT file system support for USB MSC |
|jxglib_FAT_Flash | FAT file system support for flash memory |
|jxglib_FAT_RAMDisk | FAT file system support for RAM disk |
|jxglib_LFS | LittleFS support |
|jxglib_LFS_Flash | LittleFS support for flash memory |
|jxglib_ShellCmd_FS | Shell commands for file system |
|jxglib_Image | Image support |
|jxglib_ImageFile | Image file support |
|jxglib_ShellCmd_ImageFile | Shell commands for image file |
|jxglib_Interp | Interpreter support |
|jxglib_JSON | JSON support |
|jxglib_Hash | Hashing utilities |
|jxglib_LABOPlatform | LABOPlatform support |
|jxglib_LABOPlatform_FullCmd | LABOPlatform full command support |
|jxglib_LineBuff | Line buffer support |
|jxglib_LogicAnalyzer | Logic analyzer support |
|jxglib_ShellCmd_LogicAnalyzer | Shell commands for logic analyzer |
|jxglib_LVGL | LVGL support |
|jxglib_ML | Machine learning support |
|jxglib_Mouse | Mouse support |
|jxglib_Net | Network support |
|jxglib_NetUtil | Network utilities |
|jxglib_MQTT | MQTT support |
|jxglib_Net_TCP | TCP support |
|jxglib_Net_Telnet | Telnet support |
|jxglib_Net_UDP | UDP support |
|jxglib_ShellCmd_Net | Shell commands for network |
|jxglib_ShellCmd_NetUtil | Shell commands for network utilities |
|jxglib_ShellCmd_Net_Telnet | Shell commands for Telnet |
|jxglib_ShellCmd_LED | Shell commands for LED |
|jxglib_PIO | PIO support |
|jxglib_ShellCmd_PIO | Shell commands for PIO |
|jxglib_PWM | PWM support |
|jxglib_ShellCmd_PWM | Shell commands for PWM |
|jxglib_Resets | Resets support |
|jxglib_ShellCmd_Resets | Shell commands for resets |
|jxglib_Terminal | Terminal support |
|jxglib_Shell | Shell support |
|jxglib_Serial | Serial support |
|jxglib_I2C | I2C support |
|jxglib_SPI | SPI support |
|jxglib_UART | UART support |
|jxglib_ShellCmd_I2C | Shell commands for I2C |
|jxglib_ShellCmd_SPI | Shell commands for SPI |
|jxglib_ShellCmd_UART | Shell commands for UART |
|jxglib_TouchScreen | Touch screen support |
|jxglib_TouchScreen_TSC2046 | TSC2046 touch screen support |
|jxglib_USBDevice | USB device support |
|jxglib_USBDevice_CDCSerial | USB CDC Serial support |
|jxglib_USBDevice_MSCDrive | USB MSC Drive support |
|jxglib_USBDevice_VideoTransmitter | USB Video Transmitter support |
|jxglib_USBHost | USB host support |
|jxglib_ShellCmd_USBHost_MSC | Shell commands for USB host MSC |
