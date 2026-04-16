# Shell with Serial (stdio)



## Sample Program

### Building and Flashing the Program

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

Create a new Pico SDK project named `shell-with-stdio`.

{% include-markdown "include/new-project.md" %}

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

```cmake title="CMakeLists.txt"
{% include "sample/shell-with-stdio/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `shell-with-stdio.cpp` as follows:

```cpp title="shell-with-stdio.cpp"
{% include "sample/shell-with-stdio/shell-with-stdio.cpp" %}
```

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-stdio.md" %}

A prompt will appear in the terminal like this:

```text
>
```

This firmware is linked with a library `jxglib_ShellCmd_Basic` which provides basic shell commands. Print available commands with the `help` command:

```text
>help
.               executes the given script file
about-me        prints information about this own program
about-platform  prints information about the platform
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

Dump the memory content at the address `0x10000000` with the `d` command:

```text
>d 0x10000000
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
```
