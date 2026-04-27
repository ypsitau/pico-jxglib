# Creating Shell Commands

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

Execute the `argtest` command with no arguments:

```text
>argtest
argv[0] "argtest"
```

Execute the `argtest` command with some arguments:

```text
>argtest arg1 arg2 arg3
argv[0] "argtest"
argv[1] "arg1"
argv[2] "arg2"
argv[3] "arg3"
```

Execute the `argtest` command with arguments that contain spaces:

```text
>argtest "A quick brown" "fox jumps over" "the lazy dog"
argv[0] "argtest"
argv[1] "A quick brown"
argv[2] "fox jumps over"
argv[3] "the lazy dog"
```

The custom command also appears in the help list:

```text hl_lines="5"
>help
.               executes the given script file
about-me        prints information about this own program
about-platform  prints information about the platform
argtest         tests command line arguments
d               prints memory content at the specified address
dump            prints memory content at the specified address
echo            prints the given text
echo-bin        generates binary data from the given arguments
help            prints help strings for available commands
history         prints the command history
logout          logs out the current user
password        changes the password file
prompt          changes the command line prompt
set             set environment variable
sleep           sleeps for the specified time in milliseconds
ticks           prints names and attributes of running Tickable instances
```

## Program Explanation

A shell command is defined using the `ShellCmd` macro, which has the following syntax:

```cpp
ShellCmd(symbol, "help")
{
    // command implementation
    return Result::Success;
}
```

`symbol` is the name of the command, and `"help"` is the help string that describes the command. The help string is displayed when the user types `help` command in the shell.

If the command name contains characters that are not allowed in C++ identifiers, you can use the `ShellCmd_Named` macro instead, which allows you to specify the command name as a string:

```cpp
ShellCmd_Named(symbol, "name", "help")
{
    // command implementation
    return Result::Success;
}
```

The following variables are passed to the command program:

- `Printable& tout` ... `Printable` instance. Used to puts texts to the terminal or to a file through redirection.
- `Printable& terr` ... `Printable` instance. Used to puts texts to the terminal regardless of redirection.
- `int argc` ... Number of arguments
- `char** argv` ... Argument strings. `argv[0]` contains the command name itself and `argv[1]` to `argv[argc-1]` contain the command line arguments.

The function should return `Result::Success` if there is no error, or `Result::Error` if an error occurs.

You do not need to register commands. When you create a command with the `ShellCmd` macro, it is automatically registered with the shell. With this mechanism, you can add commands simply by linking the source file that implements the command to the main program.
