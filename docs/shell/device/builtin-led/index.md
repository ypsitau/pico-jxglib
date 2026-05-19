# Built-in LED

There are many Raspberry Pi Pico compatible boards, and the built-in LEDs may differ depending on the board. There are two types of built-in LEDs: single-color LEDs and multi-color LEDs.

The official Raspberry Pi Pico board has a single-color LED, which can only be turned on or off. In pico-jxglib's shell, you can control the LED with the `led` command. [:octicons-arrow-right-24: Learn More](led-single-color.md)

Some third-party boards have multi-color LEDs, such as WS2812, which can display various colors by controlling the RGB values. You can use `ws2812` command to control it. [:octicons-arrow-right-24: Learn More](led-multi-color.md)
