# PWM Control

This article explains how to experiment with the Pico's PWM functionality using the `pwm` command in pico-jxgLABO.

PWM (Pulse Width Modulation) is a technique used for various purposes such as motor speed control and LED brightness adjustment. The Pico board can generate PWM signals on its GPIO pins, but the settings can be a bit confusing. Many people just copy and run sample programs without fully understanding them.

With pico-jxgLABO, you can interactively change parameters and observe waveforms with a logic analyzer to better understand PWM settings and operation.

## Controlling PWM with the `pwm` Command

Let's experiment with PWM using the `pwm` command. Here, Pico 2 is used, but you can do the same with the original Pico.

### Displaying Current PWM Status

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

### Specifying GPIO Pin Numbers

The first argument to the `pwm` command specifies the GPIO pin(s). You can specify a single pin or a range. Examples:

|Command         |Description                                 |
|---------------|--------------------------------------------|
|`pwm 0`        |Show PWM status for GPIO0                   |
|`pwm 2,3,8,9`  |Show PWM status for GPIO2, 3, 8, 9          |
|`pwm 2-15`     |Show PWM status for GPIO2 to GPIO15         |
|`pwm 8-`       |Show PWM status for GPIO8 to GPIO29         |

There are also shortcut commands like `pwm0` to `pwm29` (e.g., `pwm2` is the same as `pwm 2`).

### Adjusting LED Brightness with PWM

Let's control the brightness of the LED connected to GPIO25 on Pico or Pico 2. For Pico W or Pico 2 W, connect an LED to a suitable GPIO pin.

Enable PWM on GPIO25, set the frequency to 100Hz, and the duty cycle to 0.5 (50%). The `-B` option allows control of GPIO25.

```text
L:/>pwm25 -B func:pwm freq:100 duty:0.5 enable
GPIO25 func:PWM4 B     enabled  freq:100Hz (clkdiv:23.0 wrap:0xfec0) duty:0.500 (level:0x7f60) counter:0x0bd5
```

Change the duty cycle between 0 and 1 to adjust brightness:

```text
L:/>pwm25 -B duty:0
L:/>pwm25 -B duty:0.2
L:/>pwm25 -B duty:0.4
L:/>pwm25 -B duty:0.6
L:/>pwm25 -B duty:0.8
L:/>pwm25 -B duty:1
```

### Observing PWM Waveforms

The duty cycle is the ratio of high to low in the PWM signal. Use a logic analyzer to observe how the duty cycle affects the waveform.

1. Start the logic analyzer and begin measuring GPIO2, GPIO3, GPIO4, and GPIO5:

   ```text
   L:/>la -p 2-5 enable
   ```
2. Output PWM signals at 100Hz with duty cycles of 0.2, 0.4, 0.6, and 0.8:

   ```text
   L:/>pwm 2-5 func:pwm disable freq:100 counter:0
   L:/>pwm2 duty:.2; pwm3 duty:.4; pwm4 duty:.6; pwm5 duty:.8
   L:/>pwm 2-5 enable
   ```
3. Display the measured waveforms:

   ```text
   L:/>la print
   ```

The bottom waveform is duty 0.2, then 0.4, 0.6, and 0.8 above. As the duty increases, the high time increases and the pulse width widens. The period is about 10,000μs, so the frequency is 100Hz.

### Setting PWM Frequency

The `freq` subcommand changes the PWM frequency by setting the clock divider (`clkdiv`) and counter wrap value (`wrap`).

PWM uses a 16-bit counter (0 to 65535). The counter increments at a rate determined by the system clock divided by `clkdiv`, and resets to 0 when it reaches `wrap`.

#### Effect of clkdiv

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

#### Effect of wrap

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

### Maximum and Minimum Frequency

Set `clkdiv` to 1.0 and `wrap` to 1 for the highest frequency (75MHz on Pico 2). Set `clkdiv` to 255.9 and `wrap` to 65535 for the lowest frequency (about 8.9Hz on Pico 2).

### Setting PWM Duty Cycle

The duty cycle is set by the `level` value. The PWM slice sets the output high when the counter is 0, and low when it reaches `level`. If `level` is 0, the output is always low; if `level` is `wrap` + 1, the output is always high.

The duty cycle is:

$$\text{duty} = \frac{\text{level}}{\text{wrap} + 1}$$

Larger `wrap` values allow finer duty cycle resolution.

### Phase Correct PWM

Normal PWM counts up from 0 to `wrap`. Phase Correct PWM counts up to `wrap`, then down to 0, making the waveform symmetric. Enable with `phase-correct:true`.

1. Start the logic analyzer and measure GPIO2-5:

   ```text
   L:/>la -p 2-5 enable
   ```
2. Output PWM signals at 100Hz with duty cycles 0.2, 0.4, 0.6, 0.8, and enable Phase Correct:

   ```text
   L:/>pwm 2-5 func:pwm disable phase-correct:true freq:100 counter:0
   L:/>pwm2 duty:.2; pwm3 duty:.4; pwm4 duty:.6; pwm5 duty:.8
   L:/>pwm 2-5 enable
   ```
3. Display the measured waveforms:

   ```text
   L:/>la print
   ```

The waveform is symmetric about the phase reference. The frequency is halved compared to normal PWM:

$$f_{\text{PWM}} = \frac{f_{\text{sys}}}{2 \cdot \text{clkdiv} \cdot (\text{wrap} + 1)}$$

## Relation to C/C++ API

The corresponding Pico SDK APIs for each `pwm` subcommand:

| Subcommand     | Pico SDK API |
|----------------|---------------------------------------------|
| `func`         | `gpio_set_function(uint gpio, gpio_function_t fn)` |
| `enable`       | `pwm_set_enabled(uint slice_num, bool enabled)` |
| `disable`      | `pwm_set_enabled(uint slice_num, bool enabled)` |
| `freq`         | (none; calculated from `clkdiv` and `wrap`) |
| `clkdiv`       | `pwm_set_clkdiv(uint slice_num, float divider)` |
| `wrap`         | `pwm_set_wrap(uint slice_num, uint16_t wrap)` |
| `duty`         | (none; calculated from `level` and `wrap`) |
| `level`        | `pwm_set_chan_level(uint slice_num, uint chan, uint16_t level)` |
| `counter`      | `pwm_set_counter(uint slice_num, uint16_t c)` |
| `phase-correct`| `pwm_set_phase_correct(uint slice_num, bool phase_correct)` |

`pwm` のサブコマンドに相当する [pico-jxglib](https://zenn.dev/ypsitau/articles/2025-01-24-jxglib-intro) の API は以下の通りです。

| サブコマンド     | pico-jxglib API |
|-----------------|-------|
| `func`          | `GPIO::set_function(gpio_function_t fn)`|
| `enable`        | `PWM::set_enabled(bool enabled)`|
| `disable`       | `PWM::set_enabled(bool enabled)`|
| `freq`          | `PWM::set_freq(uint32_t freq)` |
| `clkdiv`        | `PWM::set_clkdiv(float divider)` |
| `wrap`          | `PWM::set_wrap(uint16_t wrap)` |
| `duty`          | `PWM::set_chan_duty(float duty)` |
| `level`         | `PWM::set_chan_level(uint16_t level)` |
| `counter`       | `PWM::set_counter(uint16_t c)` |
| `phase-correct` | `PWM::set_phase_correct(bool phase_correct)` |

## Command Reference

### pwm

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
