# Observing PWM Waveforms

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
