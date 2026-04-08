# pico-jxglib

A collection of libraries and tools for Raspberry Pi Pico, including a powerful shell and various utilities like built-in logic analyzer to enhance your development experience.

![Pico Models](docs/images/pico-pico2-picow-pico2w.jpg)

Detailed documentation: https://ypsitau.github.io/pico-jxglib/

## Motivation

Why firmware platforms have no interactive shell? When you want to modify the behavior of the firmware, you have to edit the source code, compile it, and flash it to the device. This process can be time-consuming and inefficient, especially for small changes or debugging purposes. An interactive shell allows developers to interact with the firmware in real-time, making it easier to test and modify behavior without the need for recompilation.

Yes, I know there are some interactive shells for microcontrollers, but most of them come with OS like FreeRTOS, and they are not designed for bare-metal firmware. I love bare-metal programming, and I want my programs to run as fast as possible without the overhead of an OS.

pico-jxglib aims to provide a simple but powerful interactive shell with just four lines of additional code.

## pico-jxgLABO - Ready-to-Flash UF2 Binary

If you want to try it out without setting up the development environment, you can download the ready-to-flash UF2 binary files.

|Target Board|UF2 Binary File|
|---|---|
|Raspberry Pi Pico|[pico-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico-jxgLABO.uf2)|
|Raspberry Pi Pico W|[pico-w-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico-w-jxgLABO.uf2)|
|Raspberry Pi Pico2|[pico2-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico2-jxgLABO.uf2)|
|Raspberry Pi Pico2 W|[pico2-w-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/latest/download/pico2-w-jxgLABO.uf2)|

These UF2 files are pre-compiled with the latest version of pico-jxglib and can be flashed to your Pico board using the standard UF2 flashing method. Just put your Pico into bootloader mode, copy the UF2 file to the mounted drive, and you're good to go! You can then connect to the Pico's serial console to access the interactive shell and start exploring its features.

## Built-in Logic Analyzer

While the library also comes with a rich set of built-in commands for various purposes, the most exciting feature is the built-in logic analyzer. No need to prepare and connect a logic analyzer. The Pico board that runs your firmware works as a logic analyzer!

The wave form can be visualized by two methods:

- Print it as text in the shell. This is a simple and quick way to visualize the data without needing any additional tools. [:octicons-arrow-right-24: Learn More](shell/la/text/index.md)

- Visualize it using [PulseView](https://sigrok.org/wiki/PulseView), a powerful waveform viewer. This allows you to see the captured data in a more detailed and interactive way, making it easier to analyze complex signals and timing relationships. [:octicons-arrow-right-24: Learn More](shell/la/pulseview/index.md)

Below is a demo of the logic analyzer working on the shell, capturing the I2C signals that issue READ requests for scanning devices. Please note that all the commands executed in the prompt `L:/>` are processed by the Pico firmware without the involvement of the host computer.

https://www.youtube.com/watch?v=jMSZNx5nsew

## License

pico-jxglib is licensed under the MIT License.
