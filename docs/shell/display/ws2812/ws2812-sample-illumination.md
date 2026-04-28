# Sample: Illumination

By changing the image file contents or the amount of movement of the display area, you can display various illumination patterns.

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
