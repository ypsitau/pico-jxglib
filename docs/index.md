# pico-jxglib

A collection of libraries and tools for Raspberry Pi Pico, including a powerful shell and various utilities like **built-in logic analyzer** to enhance your development experience.

## Motivation

Why firmware platforms have no interactive shell? When you want to modify the behavior of the firmware, you have to edit the source code, compile it, and flash it to the device. This process can be time-consuming and inefficient, especially for small changes or debugging purposes. An interactive shell allows developers to interact with the firmware in real-time, making it easier to test and modify behavior without the need for recompilation.

Yes, I know there are some interactive shells for microcontrollers, but most of them come with OS like FreeRTOS, and they are not designed for bare-metal firmware. I love bare-metal programming, and I want my programs to run as fast as possible without the overhead of an OS.

pico-jxglib aims to provide a simple but powerful interactive shell with just four lines of additional code. Let's see how to use it:

```cpp
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"            // Added 1

using namespace jxglib;                     // Added 2

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.Initialize();    // Added 3
	for (;;) {
        // Your code here
        Tickable::Tick();                   // Added 4
    }
}
```

Just adding these four lines makes your firmware interactive through USB's serial connection, providing a bash-like interface to execute commands.

It also comes with a rich set of built-in commands for various purposes, such as GPIO control, I2C, SPI, UART communications, and more. Among them, the most exciting feature is the built-in logic analyzer, which allows you to capture, visualize, and analyze digital signals directly from your Pico. No need to prepare and connect a logic analyzer. The Pico that runs your firmware works as a logic analyzer! This can be incredibly useful for debugging and analyzing the behavior of your circuits and peripherals.
