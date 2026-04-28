# Sample: Dot

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
