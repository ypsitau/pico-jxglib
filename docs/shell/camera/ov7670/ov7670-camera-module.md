# About the OV7670 Camera Module

For modules without FIFO memory, the OV7670 outputs image data via an 8-bit parallel data bus, synchronized by a clock. Register settings and configuration are performed via the I2C interface.

The OV7670 camera module signals are as follows:

|Signal Name|Description|
|-----|---|
|3.3V, GND|Power supply|
|PWDN |Power down control. Connect to GND for normal operation|
|RESET|Reset input. Active low. Connect to 3.3V for normal operation|
|D0-D7|8-bit parallel data bus|
|XCLK |External clock input. Usually supplied with a 24 MHz square wave|
|PCLK |Pixel clock output. Pulses for each pixel|
|HREF |Horizontal reference signal. Pixel data is valid while this is active|
|VSYNC|Vertical sync signal. Indicates the start of a frame|
|SDA  |I2C data line|
|SCL  |I2C clock line|

Even without register configuration, supplying a clock to XCLK will start image data output. The image data acquisition flow is as follows:

1. A VSYNC pulse indicates the start of a frame. Detect the transition from Low to High on VSYNC.
2. While HREF is High, read D0-D7 data on each rising edge of PCLK (Low to High transition). Pixel data is invalid while HREF is Low.
3. Repeat step 2 for the number of pixels in a frame.

Official datasheets are not always available from the manufacturer. The following PDF files were referenced for this article[^resource-link]. The Implementation Guide is especially useful, as it categorizes the register groups by function.

[^resource-link]: Since official links may break, these are hosted on my GitHub repository.

▶️ [Advanced Information Preliminary Datasheet](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-12-22-labo-ov7670/OV7670_2006.pdf)

▶️ [OV7670 Implementation Guide (V1.0)](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-12-22-labo-ov7670/OV7670%20Implementation%20Guide%20(V1.0).pdf)

Note that these documents are incomplete and some important information is missing. For example, to get color images, you need to set a register: "set register `0xB0` to `0x84`". However, the documents do not explain what register `0xB0` controls or what the value `0x84` means. For these details, you may need to refer to code written by previous developers who likely reverse-engineered the device.

