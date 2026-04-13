# Custom Commands

## `ShellCmd` Macro

To create a shell command, use the `ShellCmd` macro. The macro format is as follows:

```cpp
ShellCmd(name, "comment")
{
    // any code
    return Result::Success;
}
```

The following variables are passed to the command program:

- `Printable& tout` ... `Printable` instance. Used to puts texts to the terminal or to a file through redirection.
- `Printable& terr` ... `Printable` instance. Used to puts texts to the terminal regardless of redirection.
- `int argc` ... Number of arguments
- `char** argv` ... Argument strings. `argv[0]` contains the command name itself

Return `Result::Success` if there is no error, or `Result::Error` if an error occurs.

You do **not** need to register commands. When you create a command with the `ShellCmd` macro, it is automatically registered with the shell. With this mechanism, you can add commands simply by linking the source file that implements the command to the main program.

## Build Sample Program

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

Create a new Pico SDK project named `customcmd-argtest`.

{% include-markdown "include/new-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── customcmd-argtest/
    ├── CMakeLists.txt
    ├── customcmd-argtest.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "sample/customcmd-argtest/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `customcmd-argtest.cpp` as follows:

```cpp title="customcmd-argtest.cpp"
{% include "sample/customcmd-argtest/customcmd-argtest.cpp" %}
```

Press `F7` to build the project. Connect your Pico to the computer while holding the BOOTSEL button, and it will appear as a mass storage device, to which you can copy the generated `build/customcmd-argtest.uf2` to flash it.

## Run the Program

Connect the Pico to your computer using a USB cable and open a terminal emulator program to connect it. 

{% include-markdown "include/setup-teraterm-for-laboplatform.md" %}

Check if the `argtest` command works:

```text
L:/>argtest hello world
argv[0] "argtest"
argv[1] "hello"
argv[2] "world"
```
