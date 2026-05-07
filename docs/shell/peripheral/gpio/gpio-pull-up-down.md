### Pull-up and Pull-down

You can set pull-up or pull-down resistors to prevent GPIO pins from floating. This clarifies the signal level when the pin is unconnected.

Use `pull:up` to enable pull-up, `pull:down` for pull-down, and `pull:none` for high impedance.

The pull-up/pull-down resistance on the Pico is about 25kΩ. For I2C, the recommended pull-up is 1kΩ to 10kΩ, so the internal value is quite high. For reliable I2C, use external resistors.

!!! info
     Pico 2 has a bug where pull-down does not work correctly (see [here](https://fabscene.com/new/news/raspberry-pi-rp2350-a4-rp2354-announcement/)).

     For example, running this on Pico 2:

     ```text
     L:/>gpio 1 func:sio dir:in pull:down repeat {get sleep:300}
     ```

     The pin should always be low when unconnected, but it is unstable. Use an external resistor for reliable pull-down on Pico 2 (chip marking "P2350A0A2"). The fixed version ("P2350A0A4") works correctly.
