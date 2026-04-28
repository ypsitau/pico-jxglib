# Data Sampling

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

