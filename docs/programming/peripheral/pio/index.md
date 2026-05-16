# PIO

PIO (Programmable I/O) is a powerful feature of the Raspberry Pi Pico that allows you to create custom peripherals and interfaces. With PIO, you can offload tasks from the main CPU and achieve precise timing and control over your I/O pins.

However, programming PIO can be complex, partly because you need to create PIO assembler files and compile them with the `pioasm` tool.

To simplify the use of PIO, pico-jxglib provides a high-level C++ interface that abstracts away the complexities of PIO programming. With pico-jxglib, you can write PIO programs directly in C++ using a set of intuitive directives and instructions.

## Link and Include

Add the following line to your `CMakeLists.txt`.

```cmake title="CMakeLists.txt"
target_link_libraries(your-project jxglib_PIO)
```

Add the following line to your source file.

```cpp title="Your Source File"
#include "jxglib/PIO.h"
```
