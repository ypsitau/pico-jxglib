# GPIO

Although GPIO is often thought of as just a simple on/off input/output function, experimenting with various command operations can deepen your understanding of GPIO control. Try changing settings such as maximum current and hysteresis characteristics to explore the possibilities of the Pico.

## gpio Command

```text title="Help of the Command"
Usage: gpio [OPTION]... [PIN [COMMAND]...]
Options:
 -h --help        prints this help
 -Q --quiet       quiet mode, suppresses output of current pin status
 -B --builtin-led use the built-in LED (GPIO 25)
Sub Commands:
 sleep:MSEC           sleep for specified milliseconds
 repeat[:N] {CMD...}  repeat the commands N times (default: infinite)
 put:VALUE            drive the pin (0, 1, lo, hi)
 toggle               toggle pin value
 func:FUNCTION        set pin function (spi, uart, i2c, pwm, sio, pio0, pio1, clock, usb, xip, null)
 dir:DIR              set pin direction (in, out)
 pull:DIR             set pull direction (up, down, both, none)
 drive:STRENGTH       set pin drive strength (2mA, 4mA, 8mA, 12mA)
 slew:SLEW            set slew rate (slow, fast)
 hyst:STATE           set hysteresis state (on, off)
```

## Specifying GPIO Pin Numbers

The first argument to the `gpio` command specifies the GPIO pin number(s). You can specify a single pin or a range. Examples:

|Command         |Description                                 |
|---------------|--------------------------------------------|
|`gpio 0`       |Show status of GPIO0                        |
|`gpio 2,3,8,9` |Show status of GPIO2, 3, 8, 9               |
|`gpio 2-15`    |Show status of GPIO2 to GPIO15              |
|`gpio 8-`      |Show status of GPIO8 to GPIO29              |

There are also shortcut commands like `gpio0` to `gpio29` (e.g., `gpio2` is the same as `gpio 2`).
