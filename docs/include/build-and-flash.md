??? note "Build and Flash"
    1. Pressing `F7` on VSCode will build the project. After building, you can find the generated UF2 file in the `build` directory.
    2. Connect your Pico to the computer using a USB cable while holding the BOOTSEL button, and it will appear as a mass storage device. Copy the generated UF2 file to this device to flash it.

    If you have a debug probe like [this](../development/pico-sdk/index.md#writing-elf-files), you can also flash the program using OpenOCD and GDB. This is the recommended method for development, as it allows you to debug the program while running it on the board!
