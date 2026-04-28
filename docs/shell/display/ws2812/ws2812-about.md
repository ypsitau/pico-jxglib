# About the Full-Color Serial LED WS2812

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
