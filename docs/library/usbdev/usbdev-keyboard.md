## Building and Flashing the Program

Create a new Pico SDK project named `usbdev-keyboard`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbdev-keyboard/
    ├── CMakeLists.txt
    ├── usbdev-keyboard.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "./sample/usbdev-keyboard/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `usbdev-keyboard.cpp` as follows:

```cpp title="usbdev-keyboard.cpp"
{% include "./sample/usbdev-keyboard/usbdev-keyboard.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}
