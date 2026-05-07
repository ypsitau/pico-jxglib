# PWM Control

This article explains how to experiment with the Pico's PWM functionality using the `pwm` command in pico-jxgLABO.

PWM (Pulse Width Modulation) is a technique used for various purposes such as motor speed control and LED brightness adjustment. The Pico board can generate PWM signals on its GPIO pins, but the settings can be a bit confusing. Many people just copy and run sample programs without fully understanding them.

With pico-jxgLABO, you can interactively change parameters and observe waveforms with a logic analyzer to better understand PWM settings and operation.

## pwm Command

```text title="Help of the Command"
Usage: pwm [OPTION]... [PIN [COMMAND]...]
Options:
 -h --help        prints this help
 -n --only-pwm    only show PWM-capable pins
 -Q --quiet       do not print any status information
 -B --builtin-led use the built-in LED (GPIO 25)
Sub Commands:
 func:FUNCTION      set pin function (spi, uart, i2c, pwm, sio, pio0, pio1, clock, usb, xip, null)
 enable             start PWM output
 disable            stop PWM output
 freq:FREQUENCY     set PWM frequency in Hz
 duty:RATIO         set PWM duty ratio (0.0-1.0)
 clkdiv:DIVIDER     set PWM clock divider (1.0-255.9)
 wrap:VALUE         set PWM wrap value (0-65535)
 level:VALUE        set PWM level (0-65535)
 phase-correct:BOOL enable/disable phase-correct (0, 1)
 invert:BOOL        enable/disable inverted output (0, 1)
 counter:VALUE      set PWM counter value (0-65535)
```

## Specifying GPIO Pin Numbers

The first argument to the `pwm` command specifies the GPIO pin(s). You can specify a single pin or a range. Examples:

|Command         |Description                                 |
|---------------|--------------------------------------------|
|`pwm 0`        |Show PWM status for GPIO0                   |
|`pwm 2,3,8,9`  |Show PWM status for GPIO2, 3, 8, 9          |
|`pwm 2-15`     |Show PWM status for GPIO2 to GPIO15         |
|`pwm 8-`       |Show PWM status for GPIO8 to GPIO29         |

There are also shortcut commands like `pwm0` to `pwm29`. For example, `pwm2` is the same as `pwm 2`.
