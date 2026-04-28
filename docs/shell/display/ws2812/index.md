# Display - WS2812

In this guide, we will use the full-color serial LED WS2812 to create illumination strips and LED displays. With pico-jxgLABO, you can control WS2812 LEDs using simple commands—**no programming required**. You can turn on LEDs or display animations just by entering commands.

## Demo Video

<div class="video-container">
  <iframe 
    src="https://www.youtube.com/embed/_Ihu_z_rbcE?rel=0&modestbranding=1" 
    title="pico-jxgLABO WS2812 Demo"
    frameborder="0" 
    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" 
    allowfullscreen>
  </iframe>
</div>

## About the Full-Color Serial LED WS2812

The WS2812 is a full-color serial LED that integrates RGB LEDs and a control circuit into a single package. Each LED chip supports 24-bit color (8 bits per channel), producing vivid colors. Since it is controlled via serial communication, you can connect multiple LEDs in series and control them all with a single data line. This makes it easy to build displays and illuminations with many LEDs.

Some third-party Raspberry Pi Pico compatible boards come with built-in WS2812 LEDs, such as the Speed Studio XIAO RP2040 and Waveshare RP2040-Zero.

![speedstudio-xiao-rp2040-and-waveshare-rp2040-zero](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/speedstudio-xiao-rp2040-and-waveshare-rp2040-zero.jpg)
*Speed Studio XIAO RP2040 (left) and Waveshare RP2040-Zero (right)*

LED strips with WS2812 LEDs arranged in a line are also commercially available. Below is an example of a strip with 60 WS2812 LEDs.

![ws2812-strip](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-strip.jpg)

There are also WS2812 modules arranged in a matrix, which allow you to easily create small displays. Below is a 16x16 matrix module.

![ws2812-matrix](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix.jpg)
The shape may vary, but the control method is always the same. Connect 5V power to VCC and GND, and connect the control signal to the DIN (Data In) pin. By sending signals using pulse width modulation, you transmit 1s and 0s, and by sending 24-bit RGB data in sequence, you set the color of each LED.

![ws2812-pinout](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-pinout.jpg)

To connect multiple LEDs, connect the DOUT (Data Out) pin of one LED to the DIN pin of the next. Once a WS2812 stores its own RGB data, it passes the remaining data to the next LED via DOUT.

## Connecting the Full-Color Serial LED


Connect the VCC and GND of the WS2812 to a 5V power supply, and connect the DIN pin to a GPIO pin on the Pico board.

If you have only a small number of LEDs (up to a few dozen), you can use the Pico board's 5V output (VSYS) to power the WS2812. Below is an example where the DIN pin is connected to GPIO2.

![ws2812-circuit](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-circuit.png)

When lit at full white, each WS2812 consumes about 12mA. A strip with 60 LEDs will require about 720mA, and a 16x16 matrix module (256 LEDs) will require up to about 3A. The recommended maximum output current for the Pico board's VSYS is 300mA, which is not enough to drive many LEDs.

If you need to connect many LEDs, use an external 5V power supply as shown below. The GND of the Pico board and the external power supply must be connected together.

![ws2812-circuit-extpower](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-circuit-extpower.png)

## How to Use the Commands

pico-jxgLABO provides the `ws2812` command for direct control of WS2812 LEDs, and the `display-ws2812` command to use them as a display. Here are the basic commands.

### Direct Control of WS2812 LEDs

You can control WS2812 LEDs directly with the `ws2812` command.

First, use the `setup` subcommand to set the connection information for the WS2812. Specify the data input pin with the `din` parameter.

```text
L:/>ws2812 setup {din:PIN}
```

WS2812 LEDs are very bright, so you can adjust the brightness with the `brightness` subcommand. The brightness can be set from 0.0 (off) to 1.0 (maximum). The following sets the brightness to 0.1 (10% of maximum).

```text
L:/>ws2812 brightness:.1
```

Here are some hardware-specific examples:

- For the Speed Studio XIAO RP2040 board, the built-in WS2812 DIN pin is connected to GPIO12. The VCC is connected to GPIO11, so set this pin as a digital output and output 1.

  ```text
  L:/>ws2812 setup {din:12}
  L:/>gpio11 func:sio dir:out put:1
  ```


- For the Waveshare RP2040-Zero board, the built-in WS2812 DIN pin is connected to GPIO16. Set it as follows:

  ```text
  L:/>ws2812 setup {din:16}
  ```

- For external LED strips or matrix modules, the setup is the same. For example, if you connect these devices to GPIO2:

  ```text
  L:/>ws2812 setup {din:2}
  ```

Below, we explain how to operate using a circuit with an LED strip connected to GPIO2.

Use the `put` subcommand to set the color of the LED. For example, to light up red:

```text
L:/>ws2812 put:red
```

![ws2812-red](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-red.jpg)

You can also specify the color using a hexadecimal color code:

```text
L:/>ws2812 put:#ff0000
```

If you run the `put` subcommand multiple times, you can set the colors of multiple LEDs connected in series:

```text
L:/>ws2812 put:red put:green put:blue
```

![ws2812-rgb](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-rgb.jpg)

The colors are set in order from the LED closest to the Pico board.

### Using WS2812 LEDs as a Display


#### How to Use the `display-ws2812` Command

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

#### About the `draw` Command

To display an image on the display, use the `draw` command. The `draw` command has several drawing features, but here we introduce how to load and display an image file.

Below is an illustration of how the `draw` command works.

![draw-command-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/draw-command-image.png)

The `draw` command has an internal image buffer. When you load an image file with the `image-load` subcommand, the image data is stored in this buffer. The `image` subcommand displays the contents of the image buffer on the display.

The `image` subcommand uses the `offset` and `size` subcommands to specify the display area within the image buffer.

![draw-command-image-params](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/draw-command-image-params.png)

Other `draw` subcommands related to image drawing include:

- `offset-shift`: Shifts the display area by a specified amount
- `repeat-x`, `repeat-y`: Repeats the display area to fill the width and/or height of the display

#### Displaying a Flowing Red Dot Illumination

Let's actually display an image. Here, we use an LED strip with 60 WS2812 LEDs in series to show an animation of a flowing red dot.

1. Set up the display. Connect the DIN terminal of the LED to GPIO2.

   ```text
   L:/>display-ws2812 setup {din:2 straight:60}
   ```

2. Download the image file [red-dot.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/red-dot.png). When enlarged, the pixel image looks like this:

   ![red-dot-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/red-dot-image.png)
   *Image of `red-dot.png`*

   Save this file to the Pico board's internal storage `L:` drive. When the Pico board is connected to the host PC via USB cable, the `L:` drive appears as a USB memory device on the host PC, so copy the file there.

3. Use the `image-load` subcommand of the `draw` command to load the image file into the image buffer.

   ```text
   L:/>draw image-load:red-dot.png
   ```

4. Run the `image` subcommand to display the contents of the image buffer on the display. Here, use the `size` subcommand to set the display area in the image to 8 x 1.

   ```text
   L:/>draw image {size:8,1}
   ```

   Only one red LED lights up.

5. Using the `repeat-x` subcommand, you can repeat the image across the entire width of the display. Let's check that a red LED lights up every 8 LEDs.

   ```text
   L:/>draw image {size:8,1 repeat-x}
   ```

6. The `offset-shift` subcommand shifts the display area within the image by a specified amount. The following example shifts the display area 1 pixel to the right.

   ```text
   L:/>draw image {offset-shift:1,0}
   ```

   This operation changes the display as shown below, making it look like the dot is moving.

   ![red-dot-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/red-dot-shifted.png)

   Try running it several times to see the red LED move. When the display area reaches the right end, `offset-shift` wraps it back to the left.

7. The `repeat` subcommand repeatedly executes the subcommands inside the braces `{}`. Press `Ctrl-C` to stop the repetition.

   ```text
   L:/>draw repeat { image {offset-shift:1,0} }
   ```

   If the display speed is too fast and all LEDs appear lit, use the `sleep` subcommand to pause for the specified milliseconds. The following example shifts the display position every 100ms.

   ```text
   L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
   ```

   The illumination strip is complete!

Here is a summary of the commands to run:

```text
L:/>display-ws2812 setup {din:2 straight:60} brightness:.1
L:/>draw image-load:red-dot.png
L:/>draw image {size:8,1 repeat-x}
L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
```

#### Displaying Various Illumination Patterns

By changing the image file contents or the amount of movement of the display area, you can display various illumination patterns.

- **Hue Gradient**

  [hue-32-horz.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/hue-32-horz.png) is an image with a 32-color hue gradient pattern arranged in 2 horizontal rows.

  ![hue-32-horz-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-32-horz-image.png)
   *Image of `hue-32-horz.png`*

  Save this file to the Pico board's internal storage `L:` drive and run the following commands:

  ```text title="Illumination Bar: Hue Gradient" linenums="1"
  L:/>display-ws2812 setup {din:2 straight:60} brightness:.1
  L:/>draw image-load:hue-32-horz.png
  L:/>draw image {size:32,1 repeat-x}
  L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
  ```

  When you run `offset-shift:1,0`, the display area shifts to the right, making the hue appear to flow.

  ![hue-32-horz-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-32-horz-shifted.png)

- **Flowing Dot with Changing Color**

  [hue-256-vert.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/hue-256-vert.png) is an image with a 256-color hue gradient arranged vertically.

  ![hue-256-vert-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-256-vert-image.png)
   *Image of `hue-256-vert.png`*

  Save this file to the Pico board's internal storage `L:` drive and run the following commands:

  ```text title="Illumination Bar: Flowing Dot with Changing Color" linenums="1"
  L:/>display-ws2812 setup {din:2 straight:60} brightness:.1
  L:/>draw image-load:hue-256-vert.png
  L:/>draw image {size:8,1 repeat-x}
  L:/>draw repeat { image {offset-shift:1,1} sleep:100 }
  ```

   When you run `offset-shift:1,1`, the display area shifts diagonally down to the right, so the hue changes as the dot flows.

  ![hue-256-vert-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-256-vert-shifted.png)

  By changing the vertical shift amount in `offset-shift`, such as `offset-shift:1,2`, you can make the hue change faster.

#### Displaying Images on a WS2812 Matrix Module

Using a WS2812 module arranged in a matrix, you can easily create a small display. Below is an example of connecting a 16x16 matrix module to a Pico board.

![ws2812-matrix-connect](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-connect.jpg)

The DIN pin of the matrix module is connected to GPIO2.

- **Scrolling Text Display**

  [alphabet-white-16.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/alphabet-white-16.png) is an image file with uppercase letters A to Z in white, arranged horizontally at 16 dots high.

   ![alphabet-white-16-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/alphabet-white-16-image.png)
   *Image of `alphabet-white-16.png`*

  Save this file to the Pico board's internal storage `L:` drive and run the following commands:

   ```text title="Scrolling Text Display" linenums="1"
   L:/>display-ws2812 setup {din:2 zigzag-nw-vert:16,16} brightness:.1
   L:/>draw image-load:alphabet-white-16.png
   L:/>draw image {size:16,16}
   L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
   ```

  When you run `offset-shift:1,0`, the display area shifts to the right, making the text appear to scroll.

   ![alphabet-white-16-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/alphabet-white-16-shifted.png)

- **Animation Display**

  [rect-inflate-16.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/rect-inflate-16.png) is an image file with a square gradually inflating in a 16x16 dot area, arranged horizontally.

   ![rect-inflate-16-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/rect-inflate-16-image.png)
   *Image of `rect-inflate-16.png`*

  Save this file to the Pico board's internal storage `L:` drive and run the following commands:

   ```text title="Animation Display" linenums="1"
   L:/>display-ws2812 setup {din:2 zigzag-nw-vert:16,16} brightness:.1
   L:/>draw image-load:rect-inflate-16.png
   L:/>draw image {size:16,16}
   L:/>draw repeat { image {offset-shift:16,0} sleep:100 }
   ```

   When you run `offset-shift:16,0`, the display area shifts 16 dots to the right at a time, creating an animation effect.

   ![rect-inflate-16-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/rect-inflate-16-shifted.png)

   The command operations are exactly the same, and you can display various animations just by changing the image file.

## Automatically Displaying an Image at Power-On

To automatically display an image at power-on, create a script file named `.startup` in the Pico board's internal storage `L:` drive and write the commands you want to execute.

Below is an example of a `.startup` file to display a flowing red dot illumination bar.

```text title=".startup" linenums="1"
display-ws2812 setup {din:2 straight:60} brightness:.1
draw image-load:red-dot.png
draw image {size:8,1 repeat-x}
draw repeat { image {offset-shift:1,0} sleep:100 }
```
