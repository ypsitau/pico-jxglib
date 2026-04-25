# Creating Multiple Terminals

You can also create multiple `Terminal` instances ([program](https://github.com/ypsitau/pico-jxglib/blob/main/Terminal/test-Multi/test-Multi.cpp)).

## Wiring


## Building and Flashing the Program

Create a new Pico SDK project named `terminal-multiple`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── terminal-multiple/
    ├── CMakeLists.txt
    ├── terminal-multiple.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/terminal-multiple/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `terminal-multiple.cpp` as follows:

```cpp title="terminal-multiple.cpp" linenums="1"
{% include "./sample/terminal-multiple/terminal-multiple.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

![termtest-Multi.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-19-terminal/termtest-Multi.jpg)

Each can scroll and operate independently, making it convenient for displaying various information on a single screen.
