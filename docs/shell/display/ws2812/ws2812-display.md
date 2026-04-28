# How to Use the `display-ws2812` Command

The `display-ws2812` command allows you to use WS2812 LEDs as a display. You can display flowing patterns on an LED strip, or show images, text, and animations on a 2D matrix module.

Use the `setup` subcommand of `display-ws2812` to set the connection information and display shape. Below is an example for a strip of 60 WS2812 LEDs connected to GPIO2:

```text
L:/>display-ws2812 setup {din:2 straight:60}
```

The `din` subcommand specifies the GPIO pin connected to the DIN pin. `straight:n` means n LEDs are arranged in a straight line.

You can check the display information with the `ls-display` command:

```text
L:/>ls-display
display 0: WS2812 60x1 DIN:2 Layout:straight
```

You can see that it is recognized as a 60 x 1 pixel display.

WS2812 LEDs are very bright, so you can adjust the brightness with the `brightness` subcommand. The brightness can be set from 0.0 (off) to 1.0 (maximum). The following sets the brightness to 0.1 (10% of maximum).

```text
L:/>display-ws2812 brightness:.1
```

If you use a 2D matrix module, specify the layout as `LAYOUT-START-DIR:WIDTH,HEIGHT` instead of `straight`. Each field is specified as follows:

- `LAYOUT`: Specify `straight` or `zigzag` depending on the wiring layout.  
  ![ws2812-matrix-layout](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-layout.png)
- `START`: Specify the position of the first LED as seen from the Pico board: `nw` (north-west: top left), `ne` (north-east: top right), `sw` (south-west: bottom left), or `se` (south-east: bottom right).  
  ![ws2812-matrix-start](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-start.png)
- `DIR`: Specify the direction of each row as `vert` (vertical) or `horz` (horizontal).  
  ![ws2812-matrix-dir](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-dir.png)
- `WIDTH` and `HEIGHT`: Specify the width and height of the matrix.

For example, for a 16x16 matrix module wired in a zigzag pattern, starting from the top left and connected vertically, specify `zigzag-nw-vert:16,16`.

```text
L:/>display-ws2812 setup {din:2 zigzag-nw-vert:16,16}
L:/>ls-display
display 0: WS2812 16x16 DIN:2 Layout:zigzag-nw-vert
```
