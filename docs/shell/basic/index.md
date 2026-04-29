# Basic Commands

This page introduces the basic commands of the shell.

## help

Displays a list of available commands.

```text title="Help of the Command" linenums="1"
L:/>help --help
Usage: help [OPTION]...
Options:
 -h --help   prints this help
 -s --simple prints only command names
```

Running `help` without any options will display a list of commands along with their brief descriptions.

```text
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

## about-me

Displays information about the currently running program (build info, pin layout info embedded with the `bi_decl()` macro, and memory map). The output format is similar to picotool.

```text title="Help of the Command" linenums="1"
L:/>about-me --help
Usage: about-me [OPTION]...
Options:
 -h --help prints this help
```

Running `about-me` without any options will display the information.

```text
L:/>about-me
Program Information
 name:              shell-test
 version:           0.1
 feature:           UART stdin / stdout
 binary start:      0x10000000
 binary end:        0x1000e960

Fixed Pin Information
 0:                 UART0 TX
 1:                 UART0 RX

Build Information
 sdk version:       2.1.1
 pico_board:        pico
 boot2_name:        boot2_w25q080
 build date:        May 13 2025
 build attributes:  Debug

Memory Map
 flash binary:      0x10000000-0x1000e960   59744
 ram vector table:  0x20000000-0x200000c0     192
 data:              0x200000c0-0x20000968    2216
 bss:               0x20000968-0x20001aa4    4412
 heap:              0x20001aa4-0x20040000  255324
 stack:             0x20040000-0x20042000    8192
```

## about-platform

Displays platform information for the Pico board.

```text title="Help of the Command" linenums="1"
L:/>about-platform --help
Usage: about-platform [OPTION]...
Options:
 -h --help prints this help
```

Running `about-platform` without any options will display the information.

```text
L:/>about-platform
RP2350 (ARM) 150 MHz
Flash  0x10000000-0x10400000 4194304
SRAM   0x20000000-0x20082000  532480
```

## dump

Alias: `d`

Outputs a dump image of files or memory. The dump of file only works when a file system is mounted.

```text title="Help of the Command" linenums="1"
L:/>d --help
Usage: d [OPTION]... [ADDR [BYTES]]
       d [OPTION]... FILE
Options:
 -h --help          prints this help
    --addr-digits=n specifies address digits
    --addr          prints address
    --ascii         prints ASCII characters
    --no-addr       prints no address
    --no-ascii      prints no ASCII characters
    --no-dump       skips actual dump, just modifies parameters
 -c --row-bytes=n   specifies number of bytes per row
    --hex8          prints data in 8-bit hex format
    --hex16         prints data in 16-bit hex format
    --hex32         prints data in 32-bit hex format
    --hex16be       prints data in 16-bit big-endian hex format
    --hex32be       prints data in 32-bit big-endian hex format
```

If you specify a file name as an argument, it dumps the contents of the file in hexadecimal format.

```text
L:/>d README.txt
00  57 65 6C 63 6F 6D 65 20 74 6F 20 70 69 63 6F 2D
10  6A 78 67 4C 41 42 4F 21 0A 54 79 70 65 20 27 68
20  65 6C 70 27 20 69 6E 20 74 68 65 20 73 68 65 6C
30  6C 20 74 6F 20 73 65 65 20 61 76 61 69 6C 61 62
40  6C 65 20 63 6F 6D 6D 61 6E 64 73 2E 0A
```
The option `--ascii` prints the ASCII characters on the right side of the dump image.

```text
L:/>d README.txt --ascii
00  57 65 6C 63 6F 6D 65 20 74 6F 20 70 69 63 6F 2D  Welcome to pico-
10  6A 78 67 4C 41 42 4F 21 0A 54 79 70 65 20 27 68  jxgLABO!.Type 'h
20  65 6C 70 27 20 69 6E 20 74 68 65 20 73 68 65 6C  elp' in the shel
30  6C 20 74 6F 20 73 65 65 20 61 76 61 69 6C 61 62  l to see availab
40  6C 65 20 63 6F 6D 6D 61 6E 64 73 2E 0A           le commands..
```

If run with no arguments, it displays memory contents from address 0x00000000.

```text
L:/>d
00000000  00 1F 04 20 EB 00 00 00 35 00 00 00 31 00 00 00
00000010  4D 75 01 03 7A 00 C4 00 1D 00 00 00 00 23 02 88
00000020  9A 42 03 D0 43 88 04 30 91 42 F7 D1 18 1C 70 47
00000030  30 BF FD E7 F4 46 00 F0 05 F8 A7 48 00 21 01 60
```

If you run it again with no arguments, it displays the next block of memory.

```text
L:/>d
00000040  41 60 E7 46 A5 48 00 21 C9 43 01 60 41 60 70 47
00000050  CA 9B 0D 5B F9 1D 00 00 28 43 29 20 32 30 32 30
00000060  20 52 61 73 70 62 65 72 72 79 20 50 69 20 54 72
00000070  61 64 69 6E 67 20 4C 74 64 00 50 33 09 03 52 33
```

The first argument is the start address, and the second argument is the number of bytes to display. To specify a hexadecimal value, prefix it with `0x`.

```text
L:/>d 0x10000000
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
```

```text
L:/>d 0x10000000 128
10000000  00 B5 32 4B 21 20 58 60 98 68 02 21 88 43 98 60
10000010  D8 60 18 61 58 61 2E 4B 00 21 99 60 02 21 59 61
10000020  01 21 F0 22 99 50 2B 49 19 60 01 21 99 60 35 20
10000030  00 F0 44 F8 02 22 90 42 14 D0 06 21 19 66 00 F0
10000040  34 F8 19 6E 01 21 19 66 00 20 18 66 1A 66 00 F0
10000050  2C F8 19 6E 19 6E 19 6E 05 20 00 F0 2F F8 01 21
10000060  08 42 F9 D1 00 21 99 60 1B 49 19 60 00 21 59 60
10000070  1A 49 1B 48 01 60 01 21 99 60 EB 21 19 66 A0 21
```

## set

Sets the value of a variable. The variables set with this command are stored in memory and can be used in other commands by prefixing them with a dollar sign `$`.

```text title="Help of the Command" linenums="1"
L:/>set --help
Usage: set [OPTION]... [KEY[=VALUE]]
Options:
 -h --help prints this help
```

Below is an example of using `set` to set variables and then prints their values using `echo`.

```text
L:/>set ip_addr=192.168.0.1
L:/>set port=8000
L:/>echo "$ip_addr:$port"
192.168.0.1:8000
```

## echo

Prints the given arguments to the terminal.

```text title="Help of the Command" linenums="1"
L:/>echo --help
Usage: echo [OPTION]... [TEXT]...
Options:
 -h --help   prints this help
 -n --no-eol does not print end-of-line
```

```text
L:/>echo "Hello, World!"
Hello, World!
```

## echo-bin

Creates binary data from the given arguments and prints it to the terminal. This command is useful for creating binary files using redirection.

```text title="Help of the Command" linenums="1"
Usage: echo-bin [OPTION]... [NUMBER]...
Options:
 -h --help prints this help
 -x --hex  prints data in hex format
```

When you specify numbers as arguments, it prints the corresponding binary data.

```text
L:/>echo-bin 0x21,0x22,0x23,0x24,0x0a
!"#$
```

You can also specify ranges of numbers.

```text
L:/>echo-bin 0x40-0x7e,0x0a
@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
```

You can specify a string by enclosing it in single quotes. Escape sequences like `\n` for newline and `\t` for tab are supported.

```text
L:/>echo-bin 'Hello, World!\n'
Hello, World!
```

It works well with redirection, allowing you to easily create binary files.

```text
L:/>echo-bin 0-63 > test.bin
L:/>d test.bin
00  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
10  10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F
20  20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E 2F
30  30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F
```

Operator `*` can be used to repeat the previous value.

```text
L:/>echo-bin 0xff*16 > ff-fill.bin
L:/>d ff-fill.bin
0  FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
```

You can read other files by presenting the file name with `@` prefix.

```text
L:/>echo-bin 0xff @test.bin 0xff > test2.bin
L:/>d test2.bin
00  FF 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E
10  0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E
20  1F 20 21 22 23 24 25 26 27 28 29 2A 2B 2C 2D 2E
30  2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E
40  3F FF
```

## history

Displays the command history.

```text title="Help of the Command" linenums="1"
L:/>history --help
Usage: history [OPTION]... [NUM]...
Options:
 -h --help prints this help
```

Running `history` without any options will display the list of previously entered commands.

```text
L:/>history
1: d --help
2: dir
3: d README.txt
4: d README.txt --ascii
5: d
6: d --no-ascii
7: help --help
8: about-me --help
9: about-platform --help
10: history```

When you specify numbers as arguments, it displays the commands at those positions in the history list.

```text
L:/>history 1 3 5
d --help
d README.txt
d
```

Using this feature and redirection, you can easily create a script file from the command history.

## prompt

Changes the shell prompt. By default, the prompt displays the drive letter and the current directory path.

```text title="Help of the Command" linenums="1"
L:/>prompt --help
Usage: prompt [OPTION]... [PROMPT]
Options:
 -h --help prints this help
Variables:
  %d - current drive
  %w - current directory
  %p - platform name
  %Y - year (4 digits)
  %y - year (2 digits)
  %M - month (1-12)
  %D - day (1-31)
  %h - hour (0-23)
  %H - hour (1-12)
  %m - minute (0-59)
  %s - second (0-59)
  %A - AM/PM
```

Running the command without any arguments will print the current prompt.

```text
L:/>prompt
%d%w>
```

When specified with an argument, it changes the prompt to the given string. You can use variables in the prompt string, which will be replaced with their corresponding values when displayed.

```text
L:/>prompt "%Y-%M-%D>"
2000-01-01>
```

## sleep

Sleeps for the specified number of milliseconds.

```text title="Help of the Command" linenums="1"
L:/>sleep --help
Usage: sleep [OPTION]... [MS]
Options:
 -h --help prints this help
```

## ticks

Prints a list of active ticks. A tick is a function that is called periodically by the firmware's scheduler. This command is useful for debugging and performance analysis.

```text title="Help of the Command" linenums="1"
L:/>ticks --help
Usage: ticks [OPTION]...
Options:
 -h --help prints this help
```

Prints a list of active ticks, showing their names, priorities, and periodic execution times.

```text
L:/>ticks
Tick Called Depth Max: 2
CameraDisplayTickable          AboveNormal 0msec
Shell                          Normal 0msec
LogicAnalyzer::SigrokAdapter   Normal 0msec
Serial::Terminal               Normal 0msec
USBDevice::Controller          Normal 0msec
Flipper                        Lowest 0msec
Shell::BreakDetector           Lowest 100msec
USBDevice::MSCDrive::SyncAgent Lowest 100msec
```
