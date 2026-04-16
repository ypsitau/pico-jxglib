
## Sample Program

### Wiring

The breadboard wiring image is as follows:

![circuit-usbhost-st7789.png](images/circuit-usbhost-st7789.png)

### Building and Flashing the Program

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

Create a new Pico SDK project named `shell-with-tftlcd`.

{% include-markdown "include/new-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── shell-with-tftlcd/
    ├── CMakeLists.txt
    ├── shell-with-tftlcd.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "sample/shell-with-tftlcd/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `shell-with-tftlcd.cpp` as follows:

```cpp title="shell-with-tftlcd.cpp"
{% include "sample/shell-with-tftlcd/shell-with-tftlcd.cpp" %}
```

{% include-markdown "include/build-and-flash.md" %}
