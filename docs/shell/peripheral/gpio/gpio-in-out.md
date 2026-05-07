# Digital Input and Output

## Digital Output

Set a GPIO to output mode and output a digital signal. For example, to turn on the built-in LED (GPIO25) with a high signal:

```text
L:/>gpio 25 -B func:sio dir:out put:1
```

The `func` subcommand sets the function to `sio` (Single-cycle IO). SIO is a special block that the CPU can access in one cycle, and some of its registers are assigned to GPIO I/O. The `dir` subcommand sets output mode, and `put` sets the output value.

The `gpio` command also has `repeat` and `sleep` subcommands for repeating actions and adding delays. For example, to blink the LED:

```text
L:/>gpio 25 -B func:sio dir:out repeat:20 {put:1 sleep:100 put:0 sleep:100}
```

You can also use the `toggle` subcommand to invert the output value and blink the LED more simply:

```text
L:/>gpio 25 -B func:sio dir:out repeat:20 {toggle sleep:100}
```

You can observe GPIO signals with a logic analyzer:

```text
L:/>la enable -p 25
L:/>gpio 25 -B func:sio dir:out repeat:20 {toggle sleep:100}
L:/>la print
 Time [usec] P25 
             │  
             :  
        0.00 └─┐
               :
    99208.00 ┌─┘
             :  
   199199.52 └─┐
               :
   299194.56 ┌─┘
             :  
```

Start capture with `la enable`, operate the GPIO, then display the captured data with `la print`.

If you set the function to something other than SIO, the output value set by `put` is ignored. For example, assigning UART RX to GPIO1 and setting output high:

```text
L:/>gpio 1 func:uart dir:out put:1
GPIO1  lo~ func:UART0 RX   dir:out pull:down drive:4mA slew:slow hyst:on
```

The pin state is `lo~`, indicating the set value is ignored. The `~` means the set value and actual signal differ.

Even with SIO, if the direction is input, the set value is ignored:

```text
L:/>gpio 1 func:sio dir:in put:1
GPIO1  hi  func:SIO        dir:in  pull:down drive:4mA slew:slow hyst:on
```

It may look like the set value is reflected, but this is due to the pin state being high for another reason (e.g., a hardware bug in Pico 2). Setting the value to `0` shows it is ignored:

```text
L:/>gpio 1 func:sio dir:in put:0
GPIO1  hi~ func:SIO        dir:in  pull:down drive:4mA slew:slow hyst:on
```

**Conclusion:** To output digital signals from the CPU, set the function to SIO and the direction to output. Otherwise, the output value does not affect the pin state, so there are no side effects.

## Digital Input

Set a GPIO to input mode and read digital signals. For example, to set GPIO1 to input and read its level (try connecting it to GND with a jumper wire):

```text
L:/>gpio 1 func:sio dir:in pull:up repeat {get sleep:300}
```

The `dir` subcommand sets input mode. The `pull` subcommand enables the pull-up resistor to fix the signal high when unconnected. `get` reads the GPIO signal level.

On the Pico, you can read the digital signal level from the CPU even if the function is not SIO. For example, you can monitor UART RX on GPIO1:

```text
L:/>gpio 1 func:uart dir:in repeat {get sleep:300}
```
