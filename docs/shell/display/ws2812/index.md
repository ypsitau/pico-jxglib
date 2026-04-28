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


pico-jxgLABO provides the `ws2812` command for direct control of WS2812 LEDs, and the `display-ws2812` command to use them as a display. Here are the basic commands.





## How to Use the Commands



### Using WS2812 LEDs as a Display






## Automatically Displaying an Image at Power-On

To automatically display an image at power-on, create a script file named `.startup` in the Pico board's internal storage `L:` drive and write the commands you want to execute.

Below is an example of a `.startup` file to display a flowing red dot illumination bar.

```text title=".startup" linenums="1"
display-ws2812 setup {din:2 straight:60} brightness:.1
draw image-load:red-dot.png
draw image {size:8,1 repeat-x}
draw repeat { image {offset-shift:1,0} sleep:100 }
```
