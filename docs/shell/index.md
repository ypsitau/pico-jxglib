# Shell Commands

The shell of pico-jxglib is a powerful interactive command-line interface that allows you to interact with your firmware in real-time. It provides a bash-like experience, enabling you to execute various built-in commands for debugging, file management, and more. With the shell, you can easily test and modify the behavior of your firmware without the need for recompilation, making your development process more efficient and enjoyable.

While the shell can work with a variety of devices like USB serial, UART, Wi-Fi (Telnet), OLED display, and more, the most common use case is to use it with a USB serial. In this page, we will use pico-jxgLABO, a firmware platform that uses USB serial for the shell, as an example to demonstrate how to use the shell and its commands.

## Setting Up the Terminal

After flashing the pico-jxgLABO, connect the Pico board to your computer using a USB cable. Then, to establish a serial communication, you can use a terminal emulator program such as Tera Term (Windows), minicom (Linux), or screen (macOS).

{% include-markdown "include/setup-teraterm-for-laboplatform.md" %}

## Key Operations

The shell provides bash-like key operations to make it easier to enter commands. Below is a list of key operations that you can use in the shell.

{% include-markdown "include/key-operations.md" %}

## Help Option

Each command is equipped with a help option `--help` (or `-h`) that displays the command's detailed usage information.

```text title="Example of help option"
L:/>cp --help
Usage: cp [OPTION]... SOURCE... DEST
Options:
 -h --help      prints this help
 -r --recursive copies directories recursively
 -v --verbose   prints what is being done
 -f --force     overwrites existing files without prompting
```
