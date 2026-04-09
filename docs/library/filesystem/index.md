# File System

In embedded applications, it is common to use flash memory as a file system. Libraries like [littlefs](https://github.com/littlefs-project/littlefs) and [FatFs](https://elm-chan.org/fsw/ff/) make it relatively easy to implement a file system, but writing your own storage device handler from scratch can be a bit of a hassle. **pico-jxglib** wraps these libraries so you can easily handle file systems from Pico SDK programs.

The **pico-jxglib** file system has the following features:

- **Unified Interface**
  Supports both LittleFS and FAT as backends, allowing you to operate on files with a common interface regardless of the file system type.
- **Support for Various Storage Devices**
  Supports not only the Pico board's flash memory, but also removable media such as SD cards and USB storage.
- **Shell Command Support**
  Provides shell commands for file system operations, so you can interactively operate the file system on the Pico board.

For details on using SD cards, USB storage, shell commands, and implementing timestamps with RTC, see the following articles:
