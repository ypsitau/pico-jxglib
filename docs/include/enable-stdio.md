??? note "Enable UART and/or USB stdio"
    Find the following lines in `CMakeLists.txt`:
    
    ```cmake title="CMakeLists.txt"
    pico_enable_stdio_uart(your-project 0)
    pico_enable_stdio_usb(your-project 0)
    ```

    You can set the value to `1` to enable stdio or `0` to disable it.

    - `pico_enable_stdio_uart()` enables UART stdio that uses GPIO0 (UART0 TX) and GPIO1 (UART0 RX).
    - `pico_enable_stdio_usb()` enables USB stdio that uses the USB interface. Make sure to disable USB stdio when you use USB features because they conflict with each other.
