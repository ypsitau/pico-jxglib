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

The waveform data captured by the logic analyzer can be visualized on the shell. You can also use PulseView, a powerful waveform viewer, to analyze the captured data in more detail.

<!-- Below is a demo of the logic analyzer working on the shell, capturing the I2C signals that issue READ command for scanning devices. -->

