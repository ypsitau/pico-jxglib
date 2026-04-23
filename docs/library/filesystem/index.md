# File System

pico-jxglib provides a file system interface that abstracts away the complexities of different storage devices and file system types. This allows you to easily manage files and directories on various storage media without worrying about the underlying implementation details.

Supported storage devices are:

- **Internal Flash Memory**: The built-in flash memory of the Pico board can be used as a file system, allowing you to store data persistently without needing external storage.[:octicons-arrow-right-24: Learn More](fs-flash.md)
- **SD Cards**: You can use SD cards as removable storage, which is ideal for applications that require large amounts of data storage or the ability to easily transfer files between devices.[:octicons-arrow-right-24: Learn More](sdcard.md)
- **USB Storage**: USB flash drives can also be used as storage devices, providing a convenient way to access files on the Pico board from a computer.[:octicons-arrow-right-24: Learn More](usb-storage.md)

Supported file system types are:

- **LittleFS**: A lightweight file system designed for embedded systems, offering wear leveling and power-loss resilience. [:octicons-arrow-right-24: Learn More](fs-flash-lfs.md)
- **FAT**: A widely used file system that is compatible with many devices and operating systems, making it a good choice for SD cards and USB storage. [:octicons-arrow-right-24: Learn More](fs-flash-fat.md)

There are also shell commands available for file system operations, allowing you to interact with the file system directly from the command line. This makes it easy to manage files and directories without needing to write additional code. [:octicons-arrow-right-24: Learn More](../../shell/filesystem/file-operating-commands/index.md)
