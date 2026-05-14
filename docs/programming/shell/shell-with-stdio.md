# Shell with Stdio

Pico SDK provides a `stdio` library that allows you to use USB serial or UART as a standard input/output for your program. By attaching the shell to the `stdio`, you can easily connect it to your host computer.

## Building and Flashing the Program

Create a new Pico SDK project named `shell-with-stdio`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── shell-with-stdio/
    ├── CMakeLists.txt
    ├── shell-with-stdio.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/shell-with-stdio/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `shell-with-stdio.cpp` as follows:

```cpp title="shell-with-stdio.cpp" linenums="1"
{% include "./sample/shell-with-stdio/shell-with-stdio.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-stdio.md" %}

A prompt will appear in the terminal like this:

```text
>
```

## About the Program

```cpp linenums="11"
    Serial::Terminal terminal;
    terminal.Initialize();
```

Creates a `Serial::Terminal` object and initializes it. `Serial::Terminal` is a `Terminal` class that can be attached to a serial interface. By default, it uses `stdio` as the serial interface.

```cpp linenums="13"
    Shell::AttachTerminal(terminal);
```

Attaches the terminal to the shell. After this, you can use the shell through the terminal.
