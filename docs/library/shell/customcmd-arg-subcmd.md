# Argument Parsing - Sub Commands

Here, we will create a sample program that implements a custom shell command.

## Building and Flashing the Program

Create a new Pico SDK project named `customcmd-arg-subcmd`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── customcmd-arg-subcmd/
    ├── CMakeLists.txt
    ├── customcmd-arg-subcmd.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/customcmd-arg-subcmd/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `customcmd-arg-subcmd.cpp` as follows:

```cpp title="customcmd-arg-subcmd.cpp" linenums="1"
{% include "./sample/customcmd-arg-subcmd/customcmd-arg-subcmd.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}
