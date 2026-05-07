# Current GPIO Status

Running `gpio` with no arguments displays the current status of all GPIOs.

```text
L:/>gpio
GPIO0  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO1  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO2  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO3  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO4  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO5  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO6  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO7  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO8  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO9  lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO10 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO11 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO12 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO13 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO14 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO15 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO16 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO17 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO18 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO19 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO20 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO21 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO22 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO23*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO24*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO25*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO26 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO27 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO28 lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
GPIO29*lo  func:------     dir:in  pull:down drive:4mA slew:slow hyst:on
```

The Pico has 30 GPIOs, numbered 0 to 29. The fields in the status display mean:

- The second field, `lo` or `hi`, shows the logic value of each pin (`lo` = Low/0, `hi` = High/1)
- `func` shows the function assigned to the GPIO. `------` means no function (null function)
- `dir` is the direction: `in` for input, `out` for output
- `pull` is the pull-up/pull-down resistor setting: `down` = pull-down, `up` = pull-up, `none` = high impedance, `both` = both enabled
- `drive` is the drive strength, usually 4mA
- `slew` is slew rate control, usually `slow`
- `hyst` is input hysteresis: `on` = enabled, `off` = disabled

GPIOs marked with `*` (GPIO23, GPIO24, GPIO25, GPIO29) are not connected to external pins and are assigned to special functions:

Pico and Pico 2:
- GPIO23 - OP Controls the on-board SMPS Power Save pin
- GPIO24 - IP VBUS sense - high if VBUS is present, else low
- GPIO25 - OP Connected to user LED
- GPIO29 - IP Used in ADC mode (ADC3) to measure VSYS/3

Pico W and Pico 2 W:
- GPIO23 - OP wireless power on signal
- GPIO24 - OP/IP wireless SPI data/IRQ
- GPIO25 - OP wireless SPI CS - when high also enables GPIO29 ADC pin to read VSYS
- GPIO29 - OP/IP wireless SPI CLK/ADC mode (ADC3) to measure VSYS/3

These pins cannot be controlled by the `gpio` command to avoid affecting board operation. However, GPIO25 (user LED) can be controlled with the `-B` or `--builtin-led` option for convenience.
