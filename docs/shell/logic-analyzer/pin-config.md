# Pin Configuration

## Specifying GPIO Pins

`la` is the logic analyzer command for pico-jxgLABO. By changing the options for the `la` command, you can specify which GPIO pins to measure, the measurement target (internal/external signals), sampling rate, and more.

Here are the main options that affect integration with PulseView.

Specify the GPIO pins to measure with the `-p` (or `--pins`) option of the `la` command.

```text
L:/>la -p 2,3,4
```

GPIO pins are specified as comma-separated pin numbers. You can also specify ranges using hyphens. Examples:

|GPIO Pin Option |Description                             |
|---------------|----------------------------------------|
|`-p 0`         |Specifies GPIO0                         |
|`-p 2,3,8,9`   |Specifies GPIO2, 3, 8, 9                |
|`-p 8-15`      |Specifies GPIO8 to GPIO15               |
|`-p 2-4,8-13`  |Specifies GPIO2 to 4 and GPIO8 to 13    |

The GPIO pins specified here are reflected in the `print` subcommand and in PulseView waveform display.

## Selecting Internal or External Signals

You can specify the sampling target with the `--target` option. If you specify `internal` (default), internal Pico signals are sampled; if you specify `external`, external signals input to GPIO pins are sampled.

- `--target:internal` samples internal Pico signals
- `--target:external` samples external signals on GPIO pins

The `--target` option sets all GPIO pins at once, but you can change the setting for each GPIO pin using the following options:

- `--internal:PINS` specifies GPIO pins to sample internal signals
- `--external:PINS` specifies GPIO pins to sample external signals
- `--inherited:PINS` specifies GPIO pins to inherit the `--target` option setting

If the measurement target is external signals, a backquote (`) is added before the GPIO pin number in the status display when you run `la`. Example:

```text
L:/>la -p 2,3 --target:internal
disabled ---- 12.5MHz (samplers:1) pins:2,3 events:0/0 (heap-ratio:0.7)
L:/>la -p 2,3 --target:external
disabled ---- 12.5MHz (samplers:1) pins:`2,`3 events:0/0 (heap-ratio:0.7)
```


Use the `--target` option to specify whether to measure internal signals (inside the Pico board) or external signals (input to the Pico board's GPIO pins). If not specified, internal signals are measured by default.

|Command                                 |Description                                  |
|-----------------------------------------|---------------------------------------------|
|`la --target:external`                   |Measure external signals                     |
|`la --target:internal`                   |Measure internal signals                     |

The `--target` option sets the measurement target for all GPIO pins, but you can use the `--internal` or `--external` options to specify some GPIO pins as internal or external signals. Examples:

|Command                                               |Description                                  |
|------------------------------------------------------|---------------------------------------------|
|`la --target:external --internal:2,3,4`               |GPIO2, 3, 4 as internal, others as external  |
|`la --target:internal --external:2,3,4`               |GPIO2, 3, 4 as external, others as internal  |



## Specifying the Sampling Rate

pico-jxgLABO uses the PIO state machine for sampling. Here, the state machine used for sampling is called a sampler.

It takes 12 clock cycles for a sampler to sample one data point. For Pico2 (150MHz clock), 150MHz ÷ 12 = 12.5MHz sampling is possible. For Pico (125MHz clock), 125MHz ÷ 12 = 10.4MHz sampling is possible.

Each PIO block on the Pico board has 4 state machines (samplers). By running these in parallel with staggered start times, you can increase the sampling rate. Specify the number of samplers with the `--samplers` option. Examples:

|Option           |Pico2   |Pico     |
|-----------------|--------|---------|
|`--samplers:1`   |12.5MHz |10.4MHz  |
|`--samplers:2`   |25.0MHz |20.8MHz  |
|`--samplers:3`   |37.5MHz |31.2MHz  |
|`--samplers:4`   |50.0MHz |41.7MHz  |

Increasing the number of samplers increases the sampling rate but decreases the number of events that can be sampled.

pico-jxgLABO uses units called samplers to sample signals, and up to 4 can operate simultaneously. One sampler operates at up to 12.5MHz (Pico2) or 10.4MHz (Pico), and the total sampling rate is this rate multiplied by the number of samplers. Specify the number of samplers with the `--samplers` option. Examples:

## Specifying the PIO Block to Use

Pico2 has three blocks: PIO0, PIO1, and PIO2; Pico has two: PIO0 and PIO1. By default, PIO2 (Pico2) or PIO1 (Pico) is used for the logic analyzer, but you can specify the PIO block to use with the `--pio` option. Examples:

- `--pio:0` uses PIO0
- `--pio:1` uses PIO1
- `--pio:2` uses PIO2

## Specifying Buffer Memory Size

The `--heap-ratio:N` option (`N` is a value between 0 and 1) specifies the proportion of the heap area to use as buffer memory. The default is `0.7`, using 70% of the heap as buffer memory. If you get a memory allocation error when running `la enable`, reduce this value.

## .startup

You can use the Pico board as a dedicated logic analyzer to observe external signals. For example, running the following `la` command allows you to use a total of 24 GPIO pins (GPIO2 to GPIO22 and GPIO26 to GPIO28) as measurement pins for the logic analyzer:

```text
L:/>la -p 2-22,26-28 --target:external
```

If you create a file named `autoexec.sh` with the following content in the root directory of the `L:` drive, the Pico board will automatically configure the logic analyzer GPIO pins when powered on. This eliminates the need to operate commands from the terminal software.

```text:autoexec.sh
la -p 2-22,26-28 --target:external
```
