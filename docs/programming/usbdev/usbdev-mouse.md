## Building and Flashing the Program

Create a new Pico SDK project named `usbdev-mouse`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbdev-mouse/
    ├── CMakeLists.txt
    ├── usbdev-mouse.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/usbdev-mouse/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `usbdev-mouse.cpp` as follows:

```cpp title="usbdev-mouse.cpp" linenums="1"
{% include "./sample/usbdev-mouse/usbdev-mouse.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}
