# pico-jxglib

A collection of libraries and tools for Raspberry Pi Pico, including a powerful shell and various utilities like built-in logic analyzer to enhance your development experience.

![Pico and Pico2](images/pico-and-pico2.jpg)

## Motivation

Why firmware platforms have no interactive shell? When you want to modify the behavior of the firmware, you have to edit the source code, compile it, and flash it to the device. This process can be time-consuming and inefficient, especially for small changes or debugging purposes. An interactive shell allows developers to interact with the firmware in real-time, making it easier to test and modify behavior without the need for recompilation.

Yes, I know there are some interactive shells for microcontrollers, but most of them come with OS like FreeRTOS, and they are not designed for bare-metal firmware. I love bare-metal programming, and I want my programs to run as fast as possible without the overhead of an OS.

pico-jxglib aims to provide a simple but powerful interactive shell with just four lines of additional code.

## Actual Code

pico-jxglib is implemented in C++, but it also provides C APIs for those who prefer C. The following code snippets show how to use the library in both C++ and C.

=== "C++"
    ```cpp hl_lines="2 4 9 12" linenums="1"
    #include "pico/stdlib.h"
    #include "jxglib/LABOPlatform.h"
    
    using namespace jxglib;
    
    int main(void)
    {
    	::stdio_init_all();
    	LABOPlatform::Instance.Initialize();
    	for (;;) {
            // Your code here
            Tickable::Tick();
        }
    }
    ```
=== "C"
    ```c hl_lines="2 7 10" linenums="1"
    #include "pico/stdlib.h"
    #include "jxglib/LABOPlatform.h"
    
    int main(void)
    {
        stdio_init_all();
        jxglib_labo_init(true);
        for (;;) {
            /* Your code here */
            jxglib_tick();
        }
    }
    ```

Just adding the highlighted lines makes your firmware interactive through USB's serial connection, providing a bash-like interface to execute many powerful built-in commands for various purposes. Of course, you can also add your own custom commands to the shell. It's that simple!

## Ready-to-Flash UF2 Binary

If you want to try it out without setting up the development environment, you can download the ready-to-flash UF2 binary files.

|Target Board|UF2 Binary File|
|---|---|
|Raspberry Pi Pico|[pico-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico-jxgLABO.uf2)|
|Raspberry Pi Pico W|[pico-w-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico-w-jxgLABO.uf2)|
|Raspberry Pi Pico2|[pico2-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico2-jxgLABO.uf2)|
|Raspberry Pi Pico2 W|[pico2-w-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico2-w-jxgLABO.uf2)|

These UF2 files are pre-compiled with the latest version of pico-jxglib and can be flashed to your Pico board using the standard UF2 flashing method. Just put your Pico into bootloader mode, copy the UF2 file to the mounted drive, and you're good to go! You can then connect to the Pico's serial console to access the interactive shell and start exploring its features.

## Built-in Logic Analyzer

While the library also comes with a rich set of built-in commands for various purposes, the most exciting feature is the built-in logic analyzer, which allows you to capture, visualize, and analyze digital signals directly from your Pico. No need to prepare and connect a logic analyzer. The Pico that runs your firmware works as a logic analyzer! This can be incredibly useful for debugging and analyzing the behavior of your circuits and peripherals.

The waveform data captured by the logic analyzer can be visualized on the shell. You can also use [PulseView](https://sigrok.org/wiki/PulseView), a powerful waveform viewer, to analyze the captured data in more detail.

Below is a demo of the logic analyzer working on the shell, capturing the I2C signals that issue READ requests for scanning devices.

<div class="video-container">
  <iframe 
    src="https://www.youtube.com/embed/jMSZNx5nsew?rel=0&modestbranding=1" 
    title="pico-jxgLABO Logic Analyzer Demo"
    frameborder="0" 
    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" 
    allowfullscreen>
  </iframe>
</div>

<style>
/* Style to make the page responsive at 16:9 aspect ratio */
.video-container {
  position: relative;
  padding-bottom: 56.25%;
  height: 0;
  overflow: hidden;
  margin-bottom: 1em;
}
.video-container iframe {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  border-radius: 8px;
}
</style>

## Other Features

In addition to the interactive shell and built-in logic analyzer, pico-jxglib also provides a variety of other features, including:

- **Network Communication**: If you're using a Pico W or Pico2 W, you can also take advantage of the built-in Wi-Fi capabilities to connect to the internet and perform network communication directly from your firmware. You can remotely access the shell over the network using Telnet, allowing you to interact with your Pico from anywhere without needing a physical connection.
- **Display Support**: The library provides support for various displays such as OLED and TFT displays. You can use the shell to configure the display and draw images on the screen. WS2812 RGB LEDs can also be used as a display, allowing you to create colorful lighting effects and visualizations directly from your firmware and the shell.
- **File System on Flash Memory**: You can use the flash memory of the Pico as a file system to store and manage files. This allows you to read and write files directly from your firmware, making it easier to manage data and configurations. File operating commands, such as `dir`, `cat`, `rm`, `mv`, and `cp`, are built into the shell, allowing you to manage files directly from the command line interface. 
- **USB Mass Storage Device**: The file system above is also accessible as a USB mass storage device when the Pico is connected to a computer. This means you can easily transfer files between your computer and the Pico without needing additional software or tools.
- **USB Keyboard and Mouse Support**: You can also connect a USB keyboard and mouse to your Pico, allowing you to interact with the shell and control the display directly from the connected peripherals.
- **SD Card Support**: You can also use an SD card with your Pico. The library provides support for SD cards, allowing you to read and write files on the SD card from your firmware.
- **External USB Storage Device Support**: In addition to the built-in flash memory and SD card support, pico-jxglib also allows you to connect external USB storage devices to your Pico. This means you can use a USB flash drive or an external hard drive to store and manage files directly from your firmware.
- **Standalone Shell**: The shell can also use an OLED or TFT display as an output device and a USB keyboard as an input device, allowing you to use the shell without needing a computer connection. This can be useful for standalone applications where you want to interact with the firmware directly from the device itself.
- **Camera Support**: The library provides support for the OV7670 camera module, allowing you to capture images and video directly from your Pico. You can use the shell to configure the camera settings and capture images, which can then be saved to the file system or displayed on a connected display.
- **Real-Time Clock (RTC)**: The library also provides support for a real-time clock, allowing you to keep track of time and date in your firmware. You can use the shell to set and read the RTC, which can be useful for applications that require timekeeping or scheduling.
- **TFLite Micro Support**: The library also provides support for TensorFlow Lite Micro, allowing you to run machine learning models directly on your Pico. A C macro that includes tflite files allow you to easily integrate TFLite Micro models into your firmware.
- **LVGL Support**: The library also provides support for LVGL, a powerful graphics library for embedded systems.
- **GPIO Control**: You can control the GPIO pins of the Pico directly from the shell, allowing you to easily toggle pins and read their states.
- **PWM Control**: The library also provides support for PWM (Pulse Width Modulation), allowing you to control the brightness of LEDs, the speed of motors, and other devices that can be controlled with PWM signals directly from the shell.
- **I2C and SPI Communication**: The library provides built-in commands for I2C and SPI communication, making it easy to interact with a wide range of sensors and peripherals without needing to write complex code.
- **And much more!** The library is continuously being developed and new features are added regularly. Be sure to check the documentation and release notes for the latest updates and features.
