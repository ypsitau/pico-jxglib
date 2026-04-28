# Sample: Color Dot

By changing the image file contents or the amount of movement of the display area, you can display various illumination patterns.

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
