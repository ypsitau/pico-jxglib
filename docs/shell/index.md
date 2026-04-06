# Shell

The shell of pico-jxglib is a powerful interactive command-line interface that allows you to interact with your firmware in real-time. It provides a bash-like experience, enabling you to execute various built-in commands for debugging, file management, and more. With the shell, you can easily test and modify the behavior of your firmware without the need for recompilation, making your development process more efficient and enjoyable.

The shell can work with a variety of devices such as USB serial, UART, Wi-Fi through the Telnet protocol, and display devices like OLED and TFT LCD. You can choose any combination of these devices to create your own unique shell experience.

In this article, we will use pico-jxgLABO, a firmware platform built on top of pico-jxglib, as an example to demonstrate the features and capabilities of the shell.

## Getting Started

After flashing the pico-jxgLABO, connect the Pico board to your computer using a USB cable. Then, to establish a serial communication, you can use a terminal emulator program such as Tera Term (Windows), minicom (Linux), or screen (macOS).

Here, we will use Tera Term as an example. From the menu bar, select `[Setup]` - `[Serial port...]` to open the dialog below:

![teraterm-setting.png](images/teraterm-setting.png)

pico-jxgLABO provides two USB serial ports. On Windows, each is displayed with the following Device Instance IDs:

- `USB\VID_CAFE&PID_1AB0&MI01` ... for terminal use
- `USB\VID_CAFE&PID_1AB0&MI03` ... for applications such as logic analyzers and plotters

Select the serial port for terminal use.

When you press the `[Enter]` key in the terminal, the following prompt will appear:

```console
L:/>
```

The prompt consists of the drive letter `L:` and the current directory path. The pico-jxgLABO firmware mounts the flash memory of the Pico board as a FAT file system, and the drive letter `L:` represents this flash memory.

You can edit the input at the prompt using the cursor keys and the delete key. The `TAB` key allows you to autocomplete command names and file names. You can also browse command history with the up and down arrow keys.

First, try entering the `help` command to display a list of available commands.

```console
L:/>help
.               executes the given script file
about-me        prints information about this own program
about-platform  prints information about the platform
adc             controls ADC (Analog-to-Digital Converter)
adc0            controls ADC (Analog-to-Digital Converter)
adc1            controls ADC (Analog-to-Digital Converter)
                        :
                        :
```

If you run a command with the `--help` (or `-h`) option, detailed usage information will be displayed.

```console
L:/>cp --help
Usage: cp [OPTION]... SOURCE... DEST
Options:
 -h --help      prints this help
 -r --recursive copies directories recursively
 -v --verbose   prints what is being done
 -f --force     overwrites existing files without prompting
```

Familiar file operation commands, such as `ls`, `cp`, `mv`, `rm`, `mkdir`, and `cat`, are available. Redirection is also supported, so you can save the output of commands to a file.

The `L:` drive is also recognized as a USB mass storage device by the host PC. The drive letter assigned on the host PC may vary depending on the environment, but it is often recognized as the `D:` drive, for example. Files created on the Pico board can be accessed from the host PC, and files can also be copied from the host PC to the Pico board.

## Key Operations

The shell accepts the following key operations:

|Ctrl Key|Single Key|Function|
|---|---|---|
|Ctrl + P|Up|Show previous history|
|Ctrl + N|Down|Show next history|
|Ctrl + B|Left|Move cursor one character left|
|Ctrl + F|Right|Move cursor one character right|
|Ctrl + A|Home|Move cursor to the beginning of the line|
|Ctrl + E|End|Move cursor to the end of the line|
|Ctrl + D|Delete|Delete the character at the cursor position|
|Ctrl + H|Back|Delete the character before the cursor|
|Ctrl + J|Return|Confirm input|
|Ctrl + K| |Delete from cursor to end of line|
|Ctrl + U| |Delete from before cursor to beginning of line|

