# Setting Various Parameters

The `camera-ov7670` command allows you to set various parameters using subcommands.

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

## Command Reference

## camera-ov7670

```text title="Help of the Command"
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
 black-sun:[on|off]                 enables/disables black sun detection
 negative-image:[on|off]            enables/disables negative image
 auto-output-window:[on|off]        enables/disables auto output window
 night-mode:[on|off]                enables/disables night mode
 d56-auto-detection:[on|off]        enables/disables D56 auto detection
 auto-sharpness:[on|off]            enables/disables auto sharpness
 auto-denoise:[on|off]              enables/disables auto denoise
 dsp-color-bar:[on|off]             enables/disables DSP color bar
 uv-average:[on|off]                enables/disables UV average
 auto-contrast-center:[on|off]      enables/disables auto contrast center
 lens-correction:[on|off]           enables/disables lens correction
 black-pixel-correction:[on|off]    enables/disables black pixel correction
 white-pixel-correction:[on|off]    enables/disables white pixel correction
 rgb444:[on|off]                    enables/disables RGB444
 ablc:[on|off]                      enables/disables ABLC
 brightness:[VALUE]                 gets/sets brightness (-127 to 127)
 contrast:[VALUE]                   gets/sets contrast (0 to 255)
 sharpness:[VALUE]                  gets/sets sharpness (0 to 31)
 sharpness-max:[VALUE]              gets/sets sharpness max (0 to 31)
 sharpness-min:[VALUE]              gets/sets sharpness min (0 to 31)
 denoise:[VALUE]                    gets/sets denoise (0 to 255)
 denoise-offset:[VALUE]             gets/sets denoise offset (0 to 255)
 awb-blue:[VALUE]                   gets/sets AWB blue gain (0 to 255)
 awb-red:[VALUE]                    gets/sets AWB red gain (0 to 255)
 awb-green:[VALUE]                  gets/sets AWB green gain (0 to 255)
 exposure:[VALUE]                   gets/sets exposure (0 to 65535)
 hstart:[VALUE]                     gets/sets hstart (0 to 65535)
 hstop:[VALUE]                      gets/sets hstop (0 to 65535)
 vstart:[VALUE]                     gets/sets vstart (0 to 65535)
 vstop:[VALUE]                      gets/sets vstop (0 to 65535)
```
