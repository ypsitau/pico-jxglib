# Option Parsing

`Arg::Opt` provides a convenient way to define and parse options for custom shell commands. It supports boolean flags, string options, integer options, and float options.

## Building and Flashing the Program

Create a new Pico SDK project named `customcmd-option`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── customcmd-option/
    ├── CMakeLists.txt
    ├── customcmd-option.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/customcmd-option/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `customcmd-option.cpp` as follows:

```cpp title="customcmd-option.cpp" linenums="1"
{% include "./sample/customcmd-option/customcmd-option.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

Run the command with no arguments:

```text
>customcmd
argv[0] "customcmd"
flag  false
str   (none)
num   (none)
float (none)
```

Run the command with a bool flag:

```text
>customcmd --flag
argv[0] "customcmd"
flag  true
str   (none)
num   (none)
float (none)
```

Run the command with a string option:

```text
>customcmd --str=value1
argv[0] "customcmd"
flag  false
str   "value1"
num   (none)
float (none)
```

Run the command with an int option:

```text
>customcmd --num=1234
argv[0] "customcmd"
flag  false
str   (none)
num   1234
float (none)
```

Run the command with a float option:

```text
>customcmd --float=3.14
argv[0] "customcmd"
flag  false
str   (none)
num   (none)
float 3.140000
```

Run the command with multiple options:

```text
>customcmd --str=value1 --num=1234 --float=3.14
argv[0] "customcmd"
flag  false
str   "value1"
num   1234
float 3.140000
```

Run the command with multiple options in short form:

```text
>customcmd -s value1 -i 1234 -f 3.14
argv[0] "customcmd"
flag  false
str   "value1"
num   1234
float 3.140000
```

Print the help message:

```text
>customcmd --help
Options:
 -h --help        display help
 -b --flag        bool value
 -s --str=STR     string value
 -i --num=NUM     int value
 -f --float=FLOAT float value
```

### Program Explanation

Here is a brief explanation of the program.

```cpp linenums="9"
static const Arg::Opt optTbl[] = {
    Arg::OptBool("help",   'h', "display help"),
    Arg::OptBool("flag",   'b', "bool value"),
    Arg::OptString("str",  's', "string value", "STR"), 
    Arg::OptInt("num",     'i', "int value",    "NUM"),
    Arg::OptFloat("float", 'f', "float value",  "FLOAT"),
};
```

Define the options table using `Arg::Opt`. Each option is defined using one of the following functions, depending on the type of the option.

|Function|Description|
|-|-|
|`Arg::OptBool`|Defines a boolean option|
|`Arg::OptString`|Defines a string option|
|`Arg::OptInt`|Defines an integer option|
|`Arg::OptFloat`|Defines a float option|

- The first argument is the long name of the option, which is used in the command line as `--long-name`.
- The second argument is the short name of the option, which is used in the command line as `-s` (if the short name is 's'). If there is no short name, set it to 0.
- The third argument is the description of the option, which is displayed in the help message.
- The fourth argument is the placeholder for the option value in the help message, which is only used for non-boolean options. For example, for the `--str` option, the help message will show `--str=STR`, where `STR` is the placeholder.

```cpp linenums="16"
Arg arg(optTbl, count_of(optTbl));
if (!arg.Parse(terr, argc, argv)) return Result::Error;
```

Create an `Arg` instance and parse the command line arguments. The constructor of `Arg` takes the options table and the number of options as arguments. Then, call `arg.Parse` to parse the command line arguments. If parsing fails, return an error result. Arguments that are recognized as options will be removed from `argv`, and `argc` will be updated accordingly.

Option values can be retrieved using the following functions:

```cpp
bool defined = arg.GetBool("flag")
```

Returns `true` if the `--flag` option is specified, or `false` if it is not specified.

```cpp
const char* strValue;
bool defined = arg.GetString("str", &strValue)
```

Returns `true` if the `--str` option is specified, and sets `strValue` to the string value of the option. Returns `false` if the option is not specified.

```cpp
int intValue;
bool defined = arg.GetInt("num", &intValue)
```

Returns `true` if the `--num` option is specified, and sets `intValue` to the integer value of the option. Returns `false` if the option is not specified.

```cpp
float floatValue;
bool defined = arg.GetFloat("float", &floatValue)
```

Returns `true` if the `--float` option is specified, and sets `floatValue` to the float value of the option. Returns `false` if the option is not specified.
