# Input and Output Devices

The shell can work with a variety of devices. Below is a class diagram showing the relationship between the shell, terminal, input devices (keyboards), and output devices (displays and printables).

```mermaid
classDiagram
  {% include "include/shell-class.mmd" start="%% mkdocs-start:shell" end="%% mkdocs-end:shell" %}
  {% include "include/shell-class.mmd" start="%% mkdocs-detail-start:shell" end="%% mkdocs-detail-end:shell" %}
  classDef Class_Input fill:#ffc
  classDef Class_Output fill:#f8c
  classDef Class_Both fill:#fc9
```

<span style="padding: 5px 20px; border-width: 2px; border-style: solid; border-color: #88f; background-color: #ffc; font-size: 80%;">Input</span>
<span style="padding: 5px 20px; border-width: 2px; border-style: solid; border-color: #88f; background-color: #f8c; font-size: 80%;">Output</span>
<span style="padding: 5px 20px; border-width: 2px; border-style: solid; border-color: #88f; background-color: #fc9; font-size: 80%;">Input/Output</span>

A `Shell` can attach a `Terminal` by `Shell::AttachTerminal()`.

A `Terminal` can attach a `Keyboard` as its input device. There are various types of `Keyboard`, such as `USBHost::Keyboard`, `GPIO::Keyboard`, and `GPIO::KeyboardMatrix`. Additionally, you can also use the `Stdio`, `USBDevice::CDCSerial`, or `Telnet::Stream` as a keyboard input for the shell.

There are two types of `Terminal`: `Serial::Terminal` and `Display::Terminal`. A `Serial::Terminal` can attach a `Printable` (e.g. a `Stream`) as its output device, while a `Display::Terminal` can attach a `Display` as its output device.

A `Printable` is a class that can be printed to. It has a variety of printing methods, such as `Print()`, `Printf()`, and `Println()`. It includes classes like `Stream`, which is a base class for `Stdio`, `USBDevice::CDCSerial`, and `Telnet::Stream`.

A `Display` is a class that can be drawn on. It has methods for drawing images and bitmaps to a display device, such as `DrawImage()` and `DrawBitmap()`. It includes classes like `ST7789`, `ST7735`, `ILI9341`, `ILI9488`, and `SSD1306`.

**Conclusion**: The shell works with a variety of input and output devices by creating `Keyboard`, `Printable`, and `Display::Base` objects and attaching them to a `Terminal` attached to the `Shell`.
