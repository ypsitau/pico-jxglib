# Option Parsing

Here, we will create a sample program that implements a custom shell command.

## Building and Flashing the Program

Create a new Pico SDK project named `customcmd-definition`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── customcmd-definition/
    ├── CMakeLists.txt
    ├── customcmd-definition.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/customcmd-definition/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `customcmd-definition.cpp` as follows:

```cpp title="customcmd-definition.cpp" linenums="1"
{% include "./sample/customcmd-definition/customcmd-definition.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

### Program Explanation

`Shell::Cmd::Arg` class provides utility APIs for parsing command line arguments. You can define the expected arguments and their options in a table, and then use the `Shell::Cmd::Arg` class to parse the command line arguments based on that table. This makes it easier to handle various types of arguments, such as boolean flags, integers, floating-point numbers, and strings.

```cpp
static const Shell::Cmd::Arg::Opt optTbl[] = {
    Shell::Cmd::Arg::OptBool("help",    'h', "prints this help"),
    Shell::Cmd::Arg::OptBool("flag",    'b', "a boolean flag"),
    Shell::Cmd::Arg::OptInt("num",      'i', "specifies a number", "n"),
    Shell::Cmd::Arg::OptFloat("float",  'f', "specifies a floating point number", "n"),
    Shell::Cmd::Arg::OptString("name",  's', "specifies a name", "str"),
};
Shell::Cmd::Arg arg(optTbl, count_of(optTbl));
```
