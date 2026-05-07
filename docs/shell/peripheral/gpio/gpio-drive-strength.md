# Drive Strength

Drive strength controls the output current. The default is 4mA, but you can set 2mA, 4mA, 8mA, or 12mA with the `drive` subcommand.

To measure output current, connect an ammeter (be careful!) and run:

```text
L:/>gpio 1 func:sio dir:out pull:none drive:2mA put:1
L:/>gpio 1 func:sio dir:out pull:none drive:4mA put:1
L:/>gpio 1 func:sio dir:out pull:none drive:8mA put:1
L:/>gpio 1 func:sio dir:out pull:none drive:12mA put:1
```

To measure sink current:

```text
L:/>gpio 1 func:sio dir:out pull:none drive:2mA put:0
L:/>gpio 1 func:sio dir:out pull:none drive:4mA put:0
L:/>gpio 1 func:sio dir:out pull:none drive:8mA put:0
L:/>gpio 1 func:sio dir:out pull:none drive:12mA put:0
```

Results for Pico and Pico 2:

Output current:

|`drive` setting|Pico|Pico 2|
|:--:|:--:|:--:|
|2mA|20.2mA|20.3mA|
|4mA|30.0mA|30.0mA|
|8mA|48.0mA|48.7mA|
|12mA|56.5mA|57.1mA|

Sink current:

|`drive` setting|Pico|Pico 2|
|:--:|:--:|:--:|
|2mA|25.6mA|27.6mA|
|4mA|37.5mA|40.6mA|
|8mA|61.2mA|67.1mA|
|12mA|72.2mA|79.0mA|

The set drive strength is only a rough guide; actual current depends on load and circuit characteristics. Higher settings allow more current, which may help with high-load devices.
