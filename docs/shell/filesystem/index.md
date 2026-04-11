# File System

The shell come with a built-in file system that allows you to manage files and directories on the Pico board. You can use various commands to navigate the file system, create and delete files and directories, and perform other file operations.

You can use the following devices as storage:

**Internal Flash Memory**:
The internal flash memory that is not used for storing the firmware can be used as a file system. FAT and LittleFS file systems are supported on the flash memory.

**SD Card**:
SD cards can be connected to the Pico board using the SPI interface.

**USB Storage Device**:
When Pico works as a USB host, USB storage devices can be connected to the Pico board using the USB interface.

When the flash memory is formatted with a FAT file system, it is also accessible as a USB mass storage device when the Pico is connected to a computer, making it easy to transfer files between your computer and the Pico without needing additional software or tools.

