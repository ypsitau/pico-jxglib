This page explains how to connect an RTC module to the Pico board.

{% include "shell/device/rtc/index.md" start="<!-- mkdocs-start:about-rtc -->" end="<!-- mkdocs-end:about-rtc -->" %}

## Sample Project

### Wiring

The breadboard wiring image is as follows:

![circuit-rtc](images/circuit-rtc.png)

### Building and Flashing the Program

Create a new Pico SDK project named `rtctest`. **Check** `Console over UART` and/or `Console over USB` in `Stdio support`.

{% include-markdown "include/new-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── rtctest/
    ├── CMakeLists.txt
    ├── rtctest.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "sample/rtctest/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `rtctest.cpp` as follows:

```cpp title="rtctest.cpp"
{% include "sample/rtctest/rtctest.cpp" %}
```

{% include-markdown "include/build-and-flash.md" %}

### Running the Program

The program prints the current date and time every second. The output looks like this:

```text
2026-04-15 17:20:46
2026-04-15 17:20:47
2026-04-15 17:20:48
2026-04-15 17:20:49
2026-04-15 17:20:50
2026-04-15 17:20:51
2026-04-15 17:20:52
2026-04-15 17:20:53
2026-04-15 17:20:54
2026-04-15 17:20:55
2026-04-15 17:20:56
```
