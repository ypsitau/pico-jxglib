## Command Reference

### i2c

```text title="Help of the Command" linenums="1"
Usage: i2c BUS [OPTION]... scan
       i2c BUS [OPTION]... ADDR [COMMAND]...
  BUS: I2C bus number (0 or 1)
Options:
 -h --help          prints this help
    --help-pin      prints help for pin assignment
    --dumb          no I2C operations, just remember the pins and baudrate
 -v --verbose       verbose output for debugging
 -p --pin=SDA,SCL   sets GPIO pins for I2C
 -B --baudrate=BPS  sets I2C baudrate (default: 100000)
    --pullup=on|off enables/disables internal pull-up resistors (default: enabled)
 -t --timeout=MSEC  sets timeout for I2C operations (default: 300)
Sub Commands:
 repeat[:N] {CMD...}  repeat the commands N times (default: infinite)
 sleep:MSEC           sleep for specified milliseconds
 scan                 scan I2C bus for connected devices
 write:DATA           write DATA to I2C address ADDR
 write-c:DATA         write DATA to I2C address ADDR without STOP
 read:N               read N bytes from I2C address ADDR
 read-c:N             read N bytes from I2C address ADDR without STOP
```
