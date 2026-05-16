# GPIO

pico-jxglib provides a C++ interface for controlling GPIO pins on the Raspberry Pi Pico. With this library, you can easily configure GPIO pins for various functions, set their direction, drive strength, and more.

## Link and Include

Add the following line to your `CMakeLists.txt`.

```cmake title="CMakeLists.txt"
target_link_libraries(your-project jxglib_Common)
```

Add the following line to your source file.

```cpp title="Your Source File"
#include "jxglib/GPIO.h"
```
