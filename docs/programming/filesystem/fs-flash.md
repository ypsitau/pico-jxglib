# About Flash Memory

The Pico board's flash memory has the following address ranges:

- Pico: 0x1000'0000 - 0x1020'0000 (2MB)
- Pico2: 0x1000'0000 - 0x1040'0000 (4MB)

The program is written from the head (0x1000'0000), so the remaining flash memory can be used as a file system. To check the range occupied by the program, use one of the following methods:

!!! abstract "Check the map file"

    In the `build` directory of your project, a file like `your-project.elf.map` is generated. It should contains a symbol named `.flash_end` like the following:

    ```text title="your-project.elf.map" linenums="1"
    .flash_end      0x10005770       0x14
    ```

    The value of this symbol is the end address of the flash memory occupied by the program.

!!! abstract "Use picotool"

    picotool is a command-line tool included in the Pico SDK that can display information about the built ELF file. On Windows, you can find the executable file in `C:\Users\username\.pico-sdk\picotool\x.x.x\picotool`. Run the following command from the terminal:

    ```bash
    picotool info build/your-project.elf
    ```

    and you'll get output like:

    ```text
    File .\build\your-project.elf:
  
    Program Information
     name:          your-project
     version:       0.1
     features:      UART stdin / stdout
     binary start:  0x10000000
     binary end:    0x10005770
     target chip:   RP2350
     image type:    ARM Secure
    ```

    `binary end` is the end address of the flash memory occupied by the program.

!!! abstract "Use pico-jxglib's shell"
     If the pico-jxglib shell is running on the Pico board, you can use the `about-me` command to get information about the running program. Connect to the Pico board's shell and run the following command:

    ```text
    > about-me
    Program Information
     name:              your-project
     version:           0.1
     binary start:      0x10000000
     binary end:        0x10005770
        :
        :
    ```

You can decide the starting address of the file system by adding margin to the end address of the program. For example, if the program occupies up to 0x10005770, you can set the file system to start from 0x10008000. It depends on your expectation on how much your program will grow in the future.
