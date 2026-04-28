# Setting Various Parameters

The `camera-ov7670` command allows you to set various parameters using subcommands. Use the `--help` option to see a list of available subcommands:

```text
L:/>camera-ov7670 --help
Usage: camera-ov7670 [OPTION]... [SUBCMD...]
Options:
 -h --help prints this help
Sub Commands:
 setup                              setup a OV7670 camera module with the given parameters:
                         {i2c:BUS d0:PIN xclk:PIN pclk:PIN href:PIN vsync:PIN [pwdn:PIN] [reset:PIN] [freq:FREQ]}
 reso:RESO                          gets/sets resolution: vga, qvga, qqvga, qqqvga
 format:FORMAT                      gets/sets format: rgb565, yuv422
 dump                               dumps all OV7670 registers
 ccir656:[on|off]                   enables/disables CCIR656 mode
 scale:[on|off]                     enables/disables scaling
 dcw:[on|off]                       enables/disables DCW
 color-bar:[on|off]                 enables/disables color bar
 agc:[on|off]                       enables/disables AGC
 awb:[on|off]                       enables/disables AWB
 aec:[on|off]                       enables/disables AEC
 mirror:[on|off]                    enables/disables mirror
 vflip:[on|off]                     enables/disables vertical flip
  :
  :
```

Here are some examples of main subcommands:

- `mirror`: Enable/disable mirror (horizontal flip)

  ```text
  L:>camera-ov7670 mirror:on
  ```

- `vflip`: Enable/disable vertical flip

  ```text
  L:>camera-ov7670 vflip:on
  ```

- `brightness`: Set brightness (-127 to +127, default 0)

  ```text
  L:>camera-ov7670 brightness:-4
  ```

- `contrast`: Set contrast (0 to 255, default 64)

  ```text
  L:>camera-ov7670 contrast:15
  ```

- `sharpness`: Set sharpness (0 to 31, default 0)

  ```text
  L:>camera-ov7670 sharpness:7
  ```

- `awb`, `awb-red`, `awb-blue`, `awb-green`: Enable/disable auto white balance (AWB) and set gain for each color (0 to 255, default 128). If `awb` is set to `on`, the gains are adjusted automatically. If set to `off`, the values specified by `awb-red`, `awb-blue`, and `awb-green` are used.

  ```text
  L:>camera-ov7670 awb:off
  L:>camera-ov7670 awb-reds:120 awb-blue:140 awb-green:180
  ```
