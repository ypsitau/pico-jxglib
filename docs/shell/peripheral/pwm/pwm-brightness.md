# Controlling LED Brightness with PWM

Let's control the brightness of the built-in LED connected on Pico or Pico 2 without Wi-Fi. For Pico W or Pico 2 W, the built-in LED is not directly connected to GPIO, so you cannot control its brightness with PWM.

Enable PWM on GPIO25, set the frequency to 500Hz, and the duty cycle to 0.5 (50%). The `-B` option allows control of GPIO25.

```text
L:/>pwm25 -B func:pwm freq:500 duty:0.5 enable
GPIO25 func:PWM4 B     enabled  freq:500Hz (clkdiv:23.0 wrap:0xfec0) duty:0.500 (level:0x7f60) counter:0x0bd5
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
