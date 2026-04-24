## Building and Flashing the Program

Create a new Pico SDK project named `usbdev-msc`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbdev-msc/
    ├── CMakeLists.txt
    ├── usbdev-msc.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "./sample/usbdev-msc/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `usbdev-msc.cpp` as follows:

```cpp title="usbdev-msc.cpp"
{% include "./sample/usbdev-msc/usbdev-msc.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}
