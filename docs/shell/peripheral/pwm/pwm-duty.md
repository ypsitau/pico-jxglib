# Setting PWM Duty Cycle

The duty cycle is set by the `level` value. The PWM slice sets the output high when the counter is 0, and low when it reaches `level`. If `level` is 0, the output is always low; if `level` is `wrap` + 1, the output is always high.

The duty cycle is:

$$\text{duty} = \frac{\text{level}}{\text{wrap} + 1}$$

Larger `wrap` values allow finer duty cycle resolution.
