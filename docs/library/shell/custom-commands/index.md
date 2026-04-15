# Custom Commands

## Sample Program

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

Create a new Pico SDK project named `customcmd-argtest`. **Uncheck** `Console over USB` in `Stdio support` as this project uses LABOPlatform.

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

{% include-markdown "include/build-and-flash.md" %}

Open a terminal emulator program to connect it.

{% include-markdown "include/setup-teraterm-for-laboplatform.md" %}

Check if the `argtest` command works:

```text
L:/>argtest hello world
argv[0] "argtest"
argv[1] "hello"
argv[2] "world"
```

The custom command also appears in the help list:

```text hl_lines="5"
L:/>help
    :
adc3                      controls ADC (Analog-to-Digital Converter)
adc4                      controls ADC (Analog-to-Digital Converter)
argtest                   tests command line arguments
camera                    controls cameras
camera-ov7670             controls OV7670 camera module
    :
```

## Program Explanation

### `ShellCmd` Macro

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

### Utility APIs

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
