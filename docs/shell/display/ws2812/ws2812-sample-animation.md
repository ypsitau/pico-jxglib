# Sample: Animation

Using a WS2812 module arranged in a matrix, you can easily create a small display. Below is an example of connecting a 16x16 matrix module to a Pico board.

![ws2812-matrix-connect](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-connect.jpg)

The DIN pin of the matrix module is connected to GPIO2.

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
