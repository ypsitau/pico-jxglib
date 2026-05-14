# PIO

PIO (Programmable I/O) is a powerful feature of the Raspberry Pi Pico that allows you to create custom peripherals and interfaces. With PIO, you can offload tasks from the main CPU and achieve precise timing and control over your I/O pins.

However, programming PIO can be complex, partly because you need to create PIO assembler files and compile them with the `pioasm` tool.

To simplify the use of PIO, pico-jxglib provides a high-level C++ interface that abstracts away the complexities of PIO programming. With pico-jxglib, you can write PIO programs directly in C++ using a set of intuitive directives and instructions.
