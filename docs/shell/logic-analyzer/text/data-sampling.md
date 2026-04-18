## Starting and Stopping Data Sampling

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

After allocating buffer memory for data storage, the `la` command instructs PIO (Programmable Input/Output) and DMA (Direct Memory Access) to start sampling and then immediately exits. The actual processing is performed by PIO and DMA, so **CPU load is almost zero**. When the buffer memory is full, sampling stops automatically.

Start sampling with the `enable` subcommand.

Running the `disable` subcommand with the `la` command stops data sampling and releases PIO, DMA, and buffer memory resources. Normally, you do not need to explicitly run the `disable` subcommand.

```text
L:/>la disable
```

Running the `enable` subcommand again reinitializes resources and starts data sampling. The previous settings for GPIO pins and other sampling parameters are retained, so if there are no changes, just run `enable`.

```text
L:/>la enable
```

## About Sampling Mode

The logic analyzer in pico-jxgLABO performs sampling in transitional mode. In transitional mode, data is sampled only when the signal changes, making efficient use of buffer memory. The same sampling rate can be used for both fast (high-frequency) and slow (low-frequency) signals.

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

## Specifying the PIO Block to Use

Pico2 has three blocks: PIO0, PIO1, and PIO2; Pico has two: PIO0 and PIO1. By default, PIO2 (Pico2) or PIO1 (Pico) is used for the logic analyzer, but you can specify the PIO block to use with the `--pio` option. Examples:

- `--pio:0` uses PIO0
- `--pio:1` uses PIO1
- `--pio:2` uses PIO2

## Specifying Buffer Memory Size

The `--heap-ratio:N` option (`N` is a value between 0 and 1) specifies the proportion of the heap area to use as buffer memory. The default is `0.7`, using 70% of the heap as buffer memory. If you get a memory allocation error when running `la enable`, reduce this value.
