# Wiring

Connect the VCC and GND of the WS2812 to a 5V power supply, and connect the DIN pin to a GPIO pin on the Pico board.

If you have only a small number of LEDs (up to a few dozen), you can use the Pico board's 5V output (VSYS) to power the WS2812. Below is an example where the DIN pin is connected to GPIO2.

![ws2812-circuit](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-circuit.png)

When lit at full white, each WS2812 consumes about 12mA. A strip with 60 LEDs will require about 720mA, and a 16x16 matrix module (256 LEDs) will require up to about 3A. The recommended maximum output current for the Pico board's VSYS is 300mA, which is not enough to drive many LEDs.

If you need to connect many LEDs, use an external 5V power supply as shown below. The GND of the Pico board and the external power supply must be connected together.

![ws2812-circuit-extpower](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-circuit-extpower.png)
