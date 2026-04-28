# View UART Signals

To decode the UART protocol, specify `uart` as the protocol name for the `dec` subcommand and the following subcommands:

- `tx:PIN`: Specify the TX pin
- `rx:PIN`: Specify the RX pin
- `baudrate:RATE`: Specify the baud rate in bps (default: 115200)
- `frame:NPS`: Specify the frame format. `N` is the data bit length (5, 6, 7, 8, 9), P is parity (n:none, e:even, o:odd), S is stop bit length (1, 2). Default is `8n1` (8bit, none, 1bit stop)

`baudrate` and `frame` are optional. You must specify at least one of `tx` or `rx`.

Capture the signal when sending data from 0 to 255 on UART TX using the `uart1 write` command, and decode it with the `la dec:uart` command.

```text
L:/>la -p 4 enable
L:/>uart1 -p 4 write:0-255
L:/>la dec:uart {tx:4} print --reso:4
```

![waveform-uart-dec](images/waveform-uart-dec.png)
