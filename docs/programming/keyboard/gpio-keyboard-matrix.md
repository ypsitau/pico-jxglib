# GPIO Keyboard Matrix

## Building and Flashing the Program

Create a new Pico SDK project named `gpio-keyboard-matrix`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── gpio-keyboard-matrix/
    ├── CMakeLists.txt
    ├── gpio-keyboard-matrix.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/gpio-keyboard-matrix/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `gpio-keyboard-matrix.cpp` as follows:

```cpp title="gpio-keyboard-matrix.cpp" linenums="1"
{% include "./sample/gpio-keyboard-matrix/gpio-keyboard-matrix.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}
