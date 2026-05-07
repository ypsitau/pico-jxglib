# Current PWM Status

Running `pwm` with no arguments displays the PWM configuration for all GPIOs.

```text
L:/>pwm
GPIO0  func:------
GPIO1  func:------
GPIO2  func:------
GPIO3  func:------
GPIO4  func:------
GPIO5  func:------
GPIO6  func:------
GPIO7  func:------
GPIO8  func:------
GPIO9  func:------
GPIO10 func:------
GPIO11 func:------
GPIO12 func:------
GPIO13 func:------
GPIO14 func:------
GPIO15 func:------
GPIO16 func:------
GPIO17 func:------
GPIO18 func:------
GPIO19 func:------
GPIO20 func:------
GPIO21 func:------
GPIO22 func:------
GPIO23*func:------
GPIO24*func:------
GPIO25*func:------
GPIO26 func:------
GPIO27 func:------
GPIO28 func:------
GPIO29*func:------
```

Initially, all GPIOs show `func:------`, meaning PWM is not configured. You can set the function to PWM by specifying GPIO pin numbers and using the `func:pwm` subcommand. For example, to set all GPIOs to PWM:

```text
L:/>pwm 0- func:pwm
GPIO0  func:PWM0 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO1  func:PWM0 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO2  func:PWM1 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO3  func:PWM1 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO4  func:PWM2 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO5  func:PWM2 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO6  func:PWM3 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO7  func:PWM3 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO8  func:PWM4 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO9  func:PWM4 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO10 func:PWM5 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO11 func:PWM5 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO12 func:PWM6 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO13 func:PWM6 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO14 func:PWM7 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO15 func:PWM7 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO16 func:PWM0 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO17 func:PWM0 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO18 func:PWM1 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO19 func:PWM1 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO20 func:PWM2 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO21 func:PWM2 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO22 func:PWM3 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO23*func:------
GPIO24*func:------
GPIO25*func:------
GPIO26 func:PWM5 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO27 func:PWM5 B     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO28 func:PWM6 A     disabled freq:2288Hz (clkdiv:1.0 wrap:0xffff) duty:0.000 (level:0x0000) counter:0x0000
GPIO29*func:------
```

GPIO23, GPIO24, GPIO25, and GPIO29 are reserved for special purposes and cannot be controlled by `pwm`. However, GPIO25 (user LED) can be controlled with the `-B` or `--builtin-led` option for convenience.

All Pico GPIOs support PWM. PWM is organized into 8 slices (PWM0 to PWM7), each with two channels (A and B), for a total of 16 channels. Each slice can be enabled/disabled and have its frequency and counter settings, but channels A and B in the same slice share these settings (only the duty cycle is set individually).

To reset all functions to default:

```text
L:/>pwm 0- func:-
```
