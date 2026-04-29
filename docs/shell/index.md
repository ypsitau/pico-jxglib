# Shell

<!-- shared with library/shell/index.md -->
<!-- mkdocs-start:abstract -->
The shell of pico-jxglib is a powerful interactive command-line interface that allows you to interact with your firmware in real-time. It provides a bash-like experience, enabling you to execute various built-in commands for debugging, file management, and more. With the shell, you can easily test and modify the behavior of your firmware without the need for recompilation, making your development process more efficient and enjoyable.
<!-- mkdocs-end:abstract -->

## Flashing pico-jxgLABO

The shell is implemented in the `jxglib_Shell` library. Linking this library or other libraries that depend on it, such as `jxglib_LABOPlatform`, will make the shell available in your firmware.

<div class="grid" markdown>

Here, we will use pico-jxgLABO, a ready-to-flash UF2 Binary that links `jxglib_LABOPlatform`, as an example to demonstrate how to use the shell and its commands. It uses the USB serial interface for the shell, so you can easily try it with a single USB cable.

![shell-operation](images/shell-operation.png){:width="300px"}

</div>

Below is a list of pico-jxgLABO UF2 files:

{% include-markdown "include/uf2-list.md" %}

{% include-markdown "include/flash-uf2.md" %}

Go to [this page](../library/shell/index.md) if you are interested in the details of how to build your own firmware with the shell.

## Setting Up the Terminal

After flashing the pico-jxgLABO, connect the Pico board to your computer using a USB cable. Then, to establish a serial communication, you can use a terminal program such as Tera Term on Windows.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}

## Key Operations

The shell provides bash-like key operations to make it easier to enter commands. Below is a list of key operations that you can use in the shell.

{% include-markdown "include/key-operations.md" %}

## Help Option

Each command is equipped with a help option `--help` (or `-h`) that displays the command's detailed usage information.

```text title="Example of help option" linenums="1"
L:/>cp --help
Usage: cp [OPTION]... SOURCE... DEST
Options:
 -h --help      prints this help
 -r --recursive copies directories recursively
 -v --verbose   prints what is being done
 -f --force     overwrites existing files without prompting
```
