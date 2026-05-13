# GPIO


## Link and Include

```cmake title="CMakeLists.txt"
target_link_libraries(your-project jxglib_Common)
```

```cpp
#include "jxglib/GPIO.h"
```

## Setting Functions

Pico SDK provides a function `gpio_set_function()` to set the function of a GPIO pin. For example, to set GPIO pin 15 as SPI1 TX, you can use the following code:

```cpp
gpio_set_function(15, GPIO_FUNC_SPI);
```

OK, this seems straightforward. But, how do you know whether the SPI is SPI0 or SPI1 and what function of SPI it is? You may add a comment to clarify like follows:

```cpp
gpio_set_function(15, GPIO_FUNC_SPI); // Set as SPI1 TX
```

To add this comment, you need to carefully check the Pico SDK documentation to find out that GPIO pin 15 is SPI1 TX. This can be a bit cumbersome, especially if you have many GPIO pins to set.

Using pico-jxglib, you can write this as follows:

```cpp
GPIO15.set_function_SPI1_TX();
```

`GPIO15` is an instance of the `GPIO15_T` class, which has a member function `set_function_SPI1_TX()` that sets GPIO pin 15 as SPI1 TX.

Thanks to the code completion feature of your text editor, including VSCode, you can easily find available pin functions after typing `GPIO15.set_function_`. This makes it much easier to set the function of GPIO pins without needing to refer to the documentation.

??? note "Setting GPIO Functions"

    ```cpp title="GPIO0"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO0]" end="// mkdocs-end:[GPIO0]" %}
    ```

    ```cpp title="GPIO1"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO1]" end="// mkdocs-end:[GPIO1]" %}
    ```

    ```cpp title="GPIO2"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO2]" end="// mkdocs-end:[GPIO2]" %}
    ```

    ```cpp title="GPIO3"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO3]" end="// mkdocs-end:[GPIO3]" %}
    ```

    ```cpp title="GPIO4"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO4]" end="// mkdocs-end:[GPIO4]" %}
    ```

    ```cpp title="GPIO5"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO5]" end="// mkdocs-end:[GPIO5]" %}
    ```

    ```cpp title="GPIO6"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO6]" end="// mkdocs-end:[GPIO6]" %}
    ```

    ```cpp title="GPIO7"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO7]" end="// mkdocs-end:[GPIO7]" %}
    ```

    ```cpp title="GPIO8"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO8]" end="// mkdocs-end:[GPIO8]" %}
    ```

    ```cpp title="GPIO9"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO9]" end="// mkdocs-end:[GPIO9]" %}
    ```

    ```cpp title="GPIO10"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO10]" end="// mkdocs-end:[GPIO10]" %}
    ```

    ```cpp title="GPIO11"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO11]" end="// mkdocs-end:[GPIO11]" %}
    ```

    ```cpp title="GPIO12"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO12]" end="// mkdocs-end:[GPIO12]" %}
    ```

    ```cpp title="GPIO13"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO13]" end="// mkdocs-end:[GPIO13]" %}
    ```

    ```cpp title="GPIO14"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO14]" end="// mkdocs-end:[GPIO14]" %}
    ```

    ```cpp title="GPIO15"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO15]" end="// mkdocs-end:[GPIO15]" %}
    ```

    ```cpp title="GPIO16"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO16]" end="// mkdocs-end:[GPIO16]" %}
    ```

    ```cpp title="GPIO17"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO17]" end="// mkdocs-end:[GPIO17]" %}
    ```

    ```cpp title="GPIO18"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO18]" end="// mkdocs-end:[GPIO18]" %}
    ```

    ```cpp title="GPIO19"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO19]" end="// mkdocs-end:[GPIO19]" %}
    ```

    ```cpp title="GPIO20"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO20]" end="// mkdocs-end:[GPIO20]" %}
    ```

    ```cpp title="GPIO21"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO21]" end="// mkdocs-end:[GPIO21]" %}
    ```

    ```cpp title="GPIO22"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO22]" end="// mkdocs-end:[GPIO22]" %}
    ```

    ```cpp title="GPIO23"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO23]" end="// mkdocs-end:[GPIO23]" %}
    ```

    ```cpp title="GPIO24"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO24]" end="// mkdocs-end:[GPIO24]" %}
    ```

    ```cpp title="GPIO25"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO25]" end="// mkdocs-end:[GPIO25]" %}
    ```

    ```cpp title="GPIO26"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO26]" end="// mkdocs-end:[GPIO26]" %}
    ```

    ```cpp title="GPIO27"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO27]" end="// mkdocs-end:[GPIO27]" %}
    ```

    ```cpp title="GPIO28"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO28]" end="// mkdocs-end:[GPIO28]" %}
    ```

    ```cpp title="GPIO29"
    {% include "./sample/gpio-set-function/gpio-set-function.cpp" start="// mkdocs-start:[GPIO29]" end="// mkdocs-end:[GPIO29]" %}
    ```
