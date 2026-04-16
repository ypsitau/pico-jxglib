
## Sample Program

### Wiring

The breadboard wiring image is as follows:

![circuit-usbhost-ssd1306.png](images/circuit-usbhost-ssd1306.png)

### Building and Flashing the Program

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

Create a new Pico SDK project named `shell-with-oled`.

{% include-markdown "include/new-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── shell-with-oled/
    ├── CMakeLists.txt
    ├── shell-with-oled.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "sample/shell-with-oled/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `shell-with-oled.cpp` as follows:

```cpp title="shell-with-oled.cpp"
{% include "sample/shell-with-oled/shell-with-oled.cpp" %}
```

{% include-markdown "include/build-and-flash.md" %}
