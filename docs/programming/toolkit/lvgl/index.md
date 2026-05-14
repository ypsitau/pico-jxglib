# LVGL

[LVGL](https://docs.lvgl.io/) is a library that enables advanced GUIs on embedded devices with limited resources. It can run on as little as a 16MHz CPU, 64KB of flash, and 16KB of RAM. On a Pico with a 125MHz CPU, 2MB of flash, and 264KB of SRAM, you have plenty of headroom!

Despite its low resource requirements, LVGL offers very high expressive power. The following snapshot is from one of LVGL's sample programs:

![lvgl-bezier-anim.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-02-04-lvgl/lvgl-bezier-anim.png) (from the LVGL documentation)

When you operate the sliders, a Bezier curve is drawn in real time on the graph, with the values as control points. Clicking the play button on the right animates the red square along the curve at a speed corresponding to the curve. You can see an actual animation [here](https://docs.lvgl.io/master/examples.html#cubic-bezier-animation). It's amazing that you can implement exactly the same GUI on your own microcontroller!

What's even more impressive is that LVGL is not just for practical use, but also has a playful visual style. For example, when you click a button, you can add effects like a soft expansion. It's perfect for consumer-oriented UIs.

Porting to various platforms is also easy, following [Connecting LVGL to Your Hardware](https://docs.lvgl.io/master/intro/add-lvgl-to-your-project/connecting_lvgl.html#initializing-lvgl). **pico-jxglib** benefits from this flexibility.

