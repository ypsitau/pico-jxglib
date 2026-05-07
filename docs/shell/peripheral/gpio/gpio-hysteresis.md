### Hysteresis

Hysteresis helps filter slow or noisy signals. The default is `on`, but you can set it to `off` with the `hyst` subcommand. Disabling hysteresis may make the input unstable for slowly changing signals.

Experiment circuit:

![circuit-cr.png](images/circuit-cr.png)

The time constant is $CR=1000 \cdot 100 \cdot 10^{-6}$ = 100ms. Generate a 1000ms high pulse on GPIO0 and observe with GPIO2 and GPIO3.

First, enable hysteresis on both pins and run (use `pio1` for Pico):

```text
L:/>gpio 2,3 func:pio2 pull::none
L:/>gpio 2 hyst:on
L:/>gpio 3 hyst:on
L:/>la enable --samplers:4 -p 0,*,2,3
L:/>gpio 0 func:sio dir:out put:0 sleep:1000 put:1 sleep:1000 put:0
L:/>la print --reso:300000
```

Result:

```text
 Time [usec] P0      P2  P3  
             │       │   │  
        0.00 └─┐     │   │  
   134383.46   │     └─┐ │  
   134556.04   │       │ └─┐
               │       │   │
               │       │   │
   999803.38 ┌─┘       │   │
  1279721.92 │         │ ┌─┘
  1282086.20 │       ┌─┘ │  
 Time [usec] P0      P2  P3  
```

Next, disable hysteresis on GPIO3:

```text
L:/>gpio 2,3 func:pio2 pull::none
L:/>gpio 2 hyst:on
L:/>gpio 3 hyst:off
L:/>la enable --samplers:4 -p 0,*,2,3
L:/>gpio 0 func:sio dir:out put:0 sleep:1000 put:1 sleep:1000 put:0
L:/>la print --reso:300000
```

Result:

```text
 Time [usec] P0      P2  P3  
             │       │   │  
        0.00 └─┐     │   │  
   124398.68   │     │   └─┐
   133327.28   │     └─┐   │
               │       │   │
               │       │   │
  1000026.48 ┌─┘       │   │
  1224540.98 │         │ ┌─┘
  1275310.10 │       ┌─┘ │  
 Time [usec] P0      P2  P3  
```
