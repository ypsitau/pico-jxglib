# Built-in LED

There are many Raspberry Pi Pico compatible boards, and the built-in LEDs may differ depending on the board. There are two types of built-in LEDs: single-color LEDs and multi-color LEDs.

The official Raspberry Pi Pico board has a single-color LED, which can only be turned on or off. In pico-jxglib's shell, you can control the LED with the `led` command. [:octicons-arrow-right-24: Learn More](led-single-color.md)

XIAO RP2040 and Waveshare RP2040-Zero boards have a built-in multi-color LED (WS2812) that can display various colors by controlling the RGB values. You can use the `ws2812` command to control it.
[:octicons-arrow-right-24: Learn More](led-multi-color.md)
