## Wiring

The breadboard wiring image is as follows:

![circuit-rtc](../device/rtc/images/circuit-rtc.png)

## Building and Flashing the Program

Create a new Pico SDK project named `fs-timestamp`. **Check** `Console over UART` and/or `Console over USB` in `Stdio support`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── fs-timestamp/
    ├── CMakeLists.txt
    ├── fs-timestamp.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "./sample/fs-timestamp/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `fs-timestamp.cpp` as follows:

```cpp title="fs-timestamp.cpp"
{% include "./sample/fs-timestamp/fs-timestamp.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}
