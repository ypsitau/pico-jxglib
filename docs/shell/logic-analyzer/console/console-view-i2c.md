# View I2C Signals

To decode the I2C protocol, specify `i2c` as the protocol name for the `dec` subcommand and the following subcommands:

- `sda:PIN`: Specify the SDA pin
- `scl:PIN`: Specify the SCL pin

Capture the signal when sending Read requests to addresses 0 to 127 using the `i2c1 scan` command, and decode it with the `la dec:i2c` command.

```text
L:/>la -p 2,3 enable
L:/>i2c1 -p 2,3 scan
L:/>la dec:i2c {sda:2 scl:3} print --reso:4
```

![waveform-i2c-dec](images/waveform-i2c-dec.png)

