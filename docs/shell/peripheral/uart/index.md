## Command Reference

### uart

```text title="Help of the Command" linenums="1"
Usage: uart BUS [OPTION]... [COMMAND]...
  BUS: UART number (0 or 1)
Options:
 -h --help          prints this help
    --help-pin      prints help for pin assignment
    --dumb          no UART operations, just remember the pins and settings
 -v --verbose       verbose output for debugging
 -p --pin=TX,RX     sets GPIO pins for UART
 -b --baudrate=RATE sets UART baudrate (default: 115200)
 -f --frame=FRAME   sets frame format in DPS where D (5-8), P (n,e,o) and S (1,2) (default: 8n1)
    --flow-control  enables hardware flow control
 -t --timeout=MSEC  sets timeout for UART operations (default: 1000)
Sub Commands:
 repeat[:N] {CMD...}  repeat the commands N times (default: infinite)
 sleep:MSEC           sleep for specified milliseconds
 write:DATA           write DATA to UART
 read[:N]             read N bytes from UART (default: until timeout)
 flush                flush UART buffers
```
