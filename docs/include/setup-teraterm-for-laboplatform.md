??? note "Setup Tera Term for LABOPlatform"
    From the menu bar, select `[Setup]` - `[Serial port...]` to open the dialog below:

    ![setup-teraterm-for-laboplatform](images/setup-teraterm-for-laboplatform.png)

    pico-jxgLABO or a firmware based on LABOPlatform provides two USB serial ports. On Windows, each is displayed with the following Device Instance IDs:

    - `USB\VID_CAFE&PID_1AB0&MI01` ... for terminal use
    - `USB\VID_CAFE&PID_1AB0&MI03` ... for applications such as logic analyzers and plotters

    Select the serial port for terminal use.

    When you press the `Enter` key in the terminal, the following prompt will appear:

    ```text
    L:/>
    ```

    The prompt consists of the drive letter `L:` and the current directory path. The pico-jxgLABO firmware mounts the flash memory of the Pico board as a FAT file system, and the drive letter `L:` represents this flash memory.
