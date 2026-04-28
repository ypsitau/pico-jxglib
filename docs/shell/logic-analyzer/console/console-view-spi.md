# View SPI Signals

To decode the SPI protocol, specify `spi` as the protocol name for the `dec` subcommand and the following subcommands:

- `mode:MODE`: Specify the SPI mode (0-3)
- `mosi:PIN`: Specify the MOSI pin
- `miso:PIN`: Specify the MISO pin
- `sck:PIN`: Specify the SCK pin

You must specify at least one of `mosi` or `miso`.

Capture the signal when sending data from 0 to 255 on SPI MOSI using the `spi0 write` command, and decode it with the `la dec:spi` command.

```text
L:/>la -p 2,3 enable
L:/>spi0 -p 2,3 write:0-255
L:/>la dec:spi {mode:0 sck:2 mosi:3} print --reso:0.4
```

![waveform-spi-dec](images/waveform-spi-dec.png)
