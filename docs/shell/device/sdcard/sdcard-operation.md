# Command Operation

## File Operations

Use the `ls-drive` command to display the list of available drives:

```text
L:/>ls-drive
 Drive  Format        Total
*L:     FAT12       2621440
 M:     none              0
```

Since there is no SD card inserted yet, it shows as unmounted. Insert an SD card and run `ls-drive` again. Here, a 32 GByte SD card formatted as FAT was inserted:

```text
L:/>ls-drive
 Drive  Format        Total
*L:     FAT12       2621440
 M:     FAT32   30945574912
```

Try various file operations. See [here](../../filesystem/file-operating-commands/index.md) for a summary of file operation commands provided by the shell.

```text
L:/>M:
M:/>dir
...
```

## Command Reference

### sdcard

```text title="Help of the Command"
Usage: sdcard [OPTION]...
Options:
 -h --help prints this help
Sub Commands:
 setup  Setup an SD card with the given parameters:
          {spi:SPI cs:CS [drive:DRIVE] [baudrate:BAUDRATE]}
 init   Initialize the SD card
```
