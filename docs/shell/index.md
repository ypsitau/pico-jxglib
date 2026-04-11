# Shell Commands

The shell of pico-jxglib is a powerful interactive command-line interface that allows you to interact with your firmware in real-time. It provides a bash-like experience, enabling you to execute various built-in commands for debugging, file management, and more. With the shell, you can easily test and modify the behavior of your firmware without the need for recompilation, making your development process more efficient and enjoyable.

The shell can work with a variety of devices.

For input devices, it supports:

```mermaid
block
columns 1
  block:Input
    A("USB Serial")
    B("UART")
    C("Wi-Fi (Telnet)")
    D("USB HID Keyboard")
  end
```

For output devices, it supports:

```mermaid
block
columns 1
  block:Output
    E("USB Serial")
    F("UART")
    G("Wi-Fi (Telnet)")
    H("OLED Display")
    I("TFT LCD Display")
  end
```

You can choose any combination of these devices to create your own unique shell experience.

The most common use case of the shell is to use it with a USB serial. In this page, we will use pico-jxgLABO, a firmware platform that uses USB serial for the shell, as an example to demonstrate how to use the shell and its commands.

## Setting Up the Terminal

After flashing the pico-jxgLABO, connect the Pico board to your computer using a USB cable. Then, to establish a serial communication, you can use a terminal emulator program such as Tera Term (Windows), minicom (Linux), or screen (macOS).

Here, we will use Tera Term as an example. From the menu bar, select `[Setup]` - `[Serial port...]` to open the dialog below:

![teraterm-setting.png](images/teraterm-setting.png)

pico-jxgLABO provides two USB serial ports. On Windows, each is displayed with the following Device Instance IDs:

- `USB\VID_CAFE&PID_1AB0&MI01` ... for terminal use
- `USB\VID_CAFE&PID_1AB0&MI03` ... for applications such as logic analyzers and plotters

Select the serial port for terminal use.

When you press the `Enter` key in the terminal, the following prompt will appear:

```console
L:/>
```

The prompt consists of the drive letter `L:` and the current directory path. The pico-jxgLABO firmware mounts the flash memory of the Pico board as a FAT file system, and the drive letter `L:` represents this flash memory.

## Key Operations

The shell provides bash-like key operations to make it easier to enter commands. Below is a list of key operations that you can use in the shell.

|Ctrl Key|Single Key|Function|
|---|---|---|
|Ctrl + I|TAB|Autocomplete command and file names|
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



## Command Help

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

Explore the available commands and their options to see what you can do with the shell!
