# LaboPlatform Library

If you want to quickly incorporate pico-jxglib's functionalities into your project, you can use the `jxglib_LaboPlatform` library. This library combines multiple libraries of pico-jxglib, providing functionalities useful for the development of Pico projects. pico-jxgLABO is a ready-to-flash UF2 Binary that uses the `jxglib_LaboPlatform` library, making it a great way to quickly see the functionalities of the library in action. See [here](../../shell/index.md) for how to flash and use pico-jxgLABO.

## Building and Flashing the Program

Create a new Pico SDK project named `your-project`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── your-project/
    ├── CMakeLists.txt
    ├── your-project.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/your-project-LABOPlatform/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

- `jxglib_LaboPlatform_FullCmd` links all the libraries of useful shell commands.
- `jxglib_configure_LABOPlatform()` is required to configure libraries used by `jxglib_LaboPlatform`, such as TinyUSB and FatFS.

Edit `your-project.cpp` as follows:

```cpp title="your-project.cpp" linenums="1"
{% include "./sample/your-project-LABOPlatform/your-project.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Program Explanation

`LABOPlatform::Instance.Initialize()` initializes the platform, including the shell and all the linked libraries. It also starts a background task that runs the shell, allowing you to use the shell commands immediately after initialization.

The function runs `.startup` script internally. Therefore, if you write pin configuration commands for devices or network configuration commands in the `.startup` script, those configurations are already done after the initialization.

In default, the shell is attached to a USB serial interface, so you can use the shell through USB serial. You can also attach the shell to a UART stdio provided by Pico SDK by calling `LABOPlatform::Instance.AttachStdio().Initialize()`. In that case, you also need to edit `CMakeLists.txt` to enable the stdio as described below:

{% include-markdown "include/enable-stdio.md" %}

## Linked Libraries

The library `jxglib_LaboPlatform` links the following libraries:

|Library Name | Description|
|--- | ---|
|jxglib_Shell |A command-line interface for interacting with the system|
|jxglib_Serial |Serial communication functionalities|
|jxglib_FAT_Flash |FAT file system support for flash memory|
|jxglib_FAT_SDCard |FAT file system support for SD cards|
|jxglib_RTC_DS3231 |Support for the DS3231 real-time clock|
|jxglib_USBDevice_MSCDrive |USB Mass Storage Class (MSC) support|
|jxglib_USBDevice_CDCSerial |USB CDC Serial support|
|jxglib_USBDevice_VideoTransmitter |USB Video Transmitter support|
|jxglib_TelePlot |TelePlot support|
|jxglib_LogicAnalyzer |Logic Analyzer support|

The following libraries provide shell commands that control Pico board itself:

|Library Name | Description|
|--- | ---|
|jxglib_ShellCmd_LogicAnalyzer|`la` command that controls the built-in logic analyzer|
|jxglib_ShellCmd_FS|File system commands such as `ls`, `cat`, and `rm`|
|jxglib_ShellCmd_ADC|`adc` command that controls ADC|
|jxglib_ShellCmd_GPIO|`gpio` command that controls GPIO|
|jxglib_ShellCmd_PWM|`pwm` command that controls PWM|
|jxglib_ShellCmd_I2C|`i2c` command that controls I2C interface|
|jxglib_ShellCmd_SPI|`spi` command that controls SPI interface|
|jxglib_ShellCmd_UART|`uart` command that controls UART interface|
|jxglib_ShellCmd_Resets|`resets` command|
|jxglib_ShellCmd_LED|`led` command that controls the built-in LED|

The following libraries provide shell commands that control devices connected to the Pico board:

|Library Name | Description|
|--- | ---|
|jxglib_ShellCmd_Camera_OV7670|`camera-ov7670` command that controls OV7670 camera device|
|jxglib_ShellCmd_Display_SSD1306|`display-ssd1306` command that controls SSD1306 display|
|jxglib_ShellCmd_Display_TFT_LCD|`display-tft-lcd` command that controls TFT LCD display|
|jxglib_ShellCmd_Display_WS2812|`display-ws2812` command that controls WS2812 display|
|jxglib_ShellCmd_RTC|`rtc` command that controls real-time clock|
|jxglib_ShellCmd_RTC_DS3231|`rtc-ds3231` command that controls DS3231 real-time clock|
|jxglib_ShellCmd_Device_SDCard|`sdcard` command that controls SD card|
|jxglib_ShellCmd_Device_WS2812|`ws2812` command that controls WS2812 LEDs|

When you use Pico W or Pico2 W, the following libraries are also linked:

|Library Name | Description|
|--- | ---|
|jxglib_ShellCmd_Net|`net` command that configures network settings|
|jxglib_ShellCmd_NetUtil|Network utility commands such as `ping` and `nslookup`|
|jxglib_ShellCmd_Net_Telnet|`telnet-server` command that starts and stops the Telnet server|
