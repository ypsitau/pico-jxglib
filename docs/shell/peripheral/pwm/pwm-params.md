# Parameters

## Frequency

The `freq` subcommand changes the PWM frequency by setting the clock divider (`clkdiv`) and counter wrap value (`wrap`).

PWM uses a 16-bit counter (0 to 65535). The counter increments at a rate determined by the system clock divided by `clkdiv`, and resets to 0 when it reaches `wrap`.

### Effect of clkdiv

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

### Effect of wrap

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

## Duty Cycle

The duty cycle is set by the `level` value. The PWM slice sets the output high when the counter is 0, and low when it reaches `level`. If `level` is 0, the output is always low; if `level` is `wrap` + 1, the output is always high.

The duty cycle is:

$$\text{duty} = \frac{\text{level}}{\text{wrap} + 1}$$

Larger `wrap` values allow finer duty cycle resolution.

## Phase Correct PWM

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
