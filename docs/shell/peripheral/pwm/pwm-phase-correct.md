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
