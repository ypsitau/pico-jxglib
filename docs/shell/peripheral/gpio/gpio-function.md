# Function Settings

The familiar pinout diagram for Pico GPIO function settings:

![pico-pinout.png](images/pico-pinout.png)

See the datasheets for more details:

**Pico** - [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf) 2.19.2 Function select

![pico-function.png](images/pico-function.png)

**Pico 2** - [RP2350 Datasheet](https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf) 9.4. Function select

![pico2-function.png](images/pico2-function.png)

Each function is assigned a number from F0 to F31, with F31 being the null function (no assignment).

The `gpio` command in pico-jxgLABO uses the `func` subcommand to set the function. Available function names:

**Pico** - `spi`, `uart`, `i2c`, `pwm`, `sio`, `pio0`, `pio1`, `clock`, `usb`, `xip`, `null`
**Pico 2** - `spi`, `uart`, `uart-aux`, `i2c`, `pwm`, `sio`, `pio0`, `pio1`, `pio2`, `clock`, `usb`, `hstx`, `xip-cs1`, `coresight-trace`, `null`

For example, to set all available GPIOs to PWM function:

```text
L:/>gpio 0- func:pwm
GPIO0  lo  func:PWM0 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO1  lo  func:PWM0 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO2  lo  func:PWM1 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO3  lo  func:PWM1 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO4  lo  func:PWM2 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO5  lo  func:PWM2 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO6  lo  func:PWM3 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO7  lo  func:PWM3 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO8  lo  func:PWM4 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO9  lo  func:PWM4 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO10 lo  func:PWM5 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO11 lo  func:PWM5 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO12 lo  func:PWM6 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO13 lo  func:PWM6 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO14 lo  func:PWM7 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO15 lo  func:PWM7 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO16 lo  func:PWM0 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO17 lo  func:PWM0 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO18 lo  func:PWM1 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO19 lo  func:PWM1 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO20 lo  func:PWM2 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO21 lo  func:PWM2 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO22 lo  func:PWM3 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO23*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO24*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO25*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO26 lo  func:PWM5 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO27 lo  func:PWM5 B     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO28 lo  func:PWM6 A     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO29*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
```

Reviewing all available assignments occasionally may help you find more convenient settings.
