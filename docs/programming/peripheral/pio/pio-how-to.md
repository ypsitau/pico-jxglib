# How to Program PIO

## Building and Flashing the Program

Create a new Pico SDK project named `pio-blink`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── pio-blink/
    ├── CMakeLists.txt
    ├── pio-blink.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/pio-blink/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `pio-blink.cpp` as follows:

```cpp title="pio-blink.cpp" linenums="1"
{% include "./sample/pio-blink/pio-blink.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}
