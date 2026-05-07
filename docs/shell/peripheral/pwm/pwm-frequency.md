# Setting PWM Frequency

The `freq` subcommand changes the PWM frequency by setting the clock divider (`clkdiv`) and counter wrap value (`wrap`).

PWM uses a 16-bit counter (0 to 65535). The counter increments at a rate determined by the system clock divided by `clkdiv`, and resets to 0 when it reaches `wrap`.

## Effect of clkdiv

Set `wrap` to 1 and change `clkdiv`:

```text
L:/>pwm0 func:pwm clkdiv:1.0 wrap:1
GPIO0  func:PWM0 A     disabled freq:75000000Hz (clkdiv:1.0 wrap:0x0001) duty:0.000 (level:0x0000) counter:0x0000
L:/>pwm0 func:pwm clkdiv:2.0 wrap:1
GPIO0  func:PWM0 A     disabled freq:37500000Hz (clkdiv:2.0 wrap:0x0001) duty:0.000 (level:0x0000) counter:0x0000
L:/>pwm0 func:pwm clkdiv:3.0 wrap:1
GPIO0  func:PWM0 A     disabled freq:25000000Hz (clkdiv:3.0 wrap:0x0001) duty:0.000 (level:0x0000) counter:0x0000
```

On Pico 2, the system clock is 150MHz. With `clkdiv` 1.0 and `wrap` 1, the output is 75MHz. Increasing `clkdiv` reduces the frequency proportionally.

## Effect of wrap

Set `clkdiv` to 1.0 and change `wrap`:

```text
L:/>pwm0 func:pwm clkdiv:1.0 wrap:1
GPIO0  func:PWM0 A     disabled freq:75000000Hz (clkdiv:1.0 wrap:0x0001) duty:0.000 (level:0x0000) counter:0x0000
L:/>pwm0 func:pwm clkdiv:1.0 wrap:2
GPIO0  func:PWM0 A     disabled freq:50000000Hz (clkdiv:1.0 wrap:0x0002) duty:0.000 (level:0x0000) counter:0x0000
L:/>pwm0 func:pwm clkdiv:1.0 wrap:3
GPIO0  func:PWM0 A     disabled freq:37500000Hz (clkdiv:1.0 wrap:0x0003) duty:0.000 (level:0x0000) counter:0x0000
```

With `clkdiv` 1.0, increasing `wrap` decreases the frequency. The frequency is calculated as:

$$f_{\text{PWM}} = \frac{f_{\text{sys}}}{\text{clkdiv} \cdot (\text{wrap} + 1)}$$

## Maximum and Minimum Frequency

Set `clkdiv` to 1.0 and `wrap` to 1 for the highest frequency (75MHz on Pico 2). Set `clkdiv` to 255.9 and `wrap` to 65535 for the lowest frequency (about 8.9Hz on Pico 2).
