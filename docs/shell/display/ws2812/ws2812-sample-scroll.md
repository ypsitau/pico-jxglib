# Sample: Scrolling Text

Using a WS2812 module arranged in a matrix, you can easily create a small display. Below is an example of connecting a 16x16 matrix module to a Pico board.

![ws2812-matrix-connect](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-connect.jpg)

The DIN pin of the matrix module is connected to GPIO2.

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
