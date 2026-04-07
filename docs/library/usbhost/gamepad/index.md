This page explains how to connect a USB gamepad to the Pico board using its USB host functionality. Enjoy gaming and more!

## About USB Gamepads

Gamepads are devices originally designed for gaming, but they are also useful as compact multi-button controllers for robotics and other applications. They typically offer two analog joysticks (left and right), D-pad input, and more than 10 buttons, all in a form factor that fits comfortably in your hands. The fact that you can get these for just a few thousand yen is very appealing.

However, many manufacturers have produced gamepads with various specifications, so the standards are quite mixed. To help you choose, here is a brief summary of gamepad specifications.

### DirectInput and XInput

There are two main interface standards for USB gamepads: **DirectInput** and **XInput**. **pico-jxglib** supports **DirectInput**.

**DirectInput** is an older standard implemented as a USB HID class. The HID class supports a variety of human interface devices (keyboards, mice, buttons, analog joysticks, touch panels, pointing devices, etc.) and allows the device to define its own data format using a Report Descriptor. This flexibility means you can create devices with any button layout or number, but it also leads to a lack of uniformity. Even within the same brand, the bit layout and usage values in the Report Descriptor can differ between models. The SDL library for PC use provides a gamepad API and absorbs these differences using a [device-specific database](https://github.com/mdqinc/SDL_GameControllerDB).

**XInput** is a standard proposed by Microsoft for Xbox gamepads. It strictly defines the layout and number of buttons and joysticks. However, it is not a HID class, but a vendor-specific class (Vendor ID `0x045E` for Microsoft, Product ID `0x028E` for Xbox360 Controller, etc.). To use XInput devices, you may need to install an INF file to register your own Vendor and Product IDs with the OS[^vendorid].

[^vendorid]: Some gamepads use Microsoft's Vendor and Product IDs without permission. This may allow easy connection without installation, but it is technically device impersonation and could cause issues.

Many gamepads provided by third-party vendors can switch between DirectInput and XInput modes. However, gamepads sold as Xbox controllers only support XInput.

### USB Gamepads Usable with the Pico Board

Not all USB gamepads on the market can be connected to the Pico board. I tried three gamepads from different brands, but only one worked.

- The EasySMX ESM-9013 wireless type was not recognized even when connected directly. It was recognized after resetting the board while connected, but was unstable when using a USB hub. Once connected, reports were received correctly.
- The Logicool F310 wired type is a major product, but I could not get it to work. The hardware connection and descriptor acquisition succeeded, but no reports were received. Sometimes it worked after some trigger, so I suspect a fix in tinyusb could solve it, but the cause is unclear.
- **The Elecom JC-U4113S wired type** was the only gamepad that worked reliably in my tests.

## About the Report Descriptor Parser

USB HID devices send data called reports to the host at regular intervals. The format of this data is defined by the Report Descriptor, which specifies the position, min/max values, and usage of each field (buttons, joysticks, etc.).

Supporting various devices requires a somewhat complex parser. For this project, I implemented a parser that can handle the Report Descriptor and extract all relevant information. This should allow support for a wide range of gamepads, but the parser could also be used for other purposes, so I plan to document it separately in detail.

## Creating a Monitor Program

Let's create a monitor program that displays the button and joystick information from a gamepad on a TFT LCD.

### Wiring Example

Here is the breadboard wiring for using an ILI9341 TFT LCD:

![circuit-ili9341.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/circuit-ili9341.png)

Here is the breadboard wiring for using an ST7789 TFT LCD:

![circuit-st7789.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/circuit-st7789.png)

### Creating a Project

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings. For details on creating a Pico SDK project, building, and writing to the board, see ["Getting Started with Pico SDK"](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86).

- **Name** ... Enter the project name. In this example, enter `usbhost-gamepad-monitor`.
- **Board type** ... Select the board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Stdio support** ... Leave both unchecked.
- **Code generation options** ... **Check `Generate C++ code`**

Assume the project directory and `pico-jxglib` are arranged as follows:

```text
├── pico-jxglib/
└── usbhost-gamepad-monitor/
    ├── CMakeLists.txt
    ├── usbhost-gamepad-monitor.cpp
    └── ...
```

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(usbhost-gamepad-monitor jxglib_USBHost jxglib_Display_ILI9341 jxglib_Display_ST7789)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhost-gamepad-monitor CFG_TUH_HID 3)
```

## Using the Gamepad

The `USBHost::GamePad` instance is created to get information from the gamepad. The `Get_Button0()` to `Get_Button12()` functions return true or false depending on whether the button is pressed. The `Get_Axis0()` to `Get_Axis8()` functions return the amount of movement for the analog joysticks, ranging from -1 to +1.

## Playing Games

Since we have a gamepad, let's enjoy some games. [KenKen さん](http://www.ze.em-net.ne.jp/~kenken/index.html) has [Raspberry Pi Pico による液晶ゲーム製作](http://www.ze.em-net.ne.jp/~kenken/picogames/index.html) where you can find high-quality games that run on the Pico board. We can play these games using the gamepad.

The mapping of each button and joystick is as follows. As mentioned earlier, the actual location of each button or analog joystick on the gamepad may vary by model, but this mapping should be roughly correct (except for the right analog joystick).

- Left and right analog joysticks ... Up, Down, Left, Right
- D-pad ... Up, Down, Left, Right
- A, B, X, Y buttons ... Fire
- Other buttons ... Start

The wiring is as shown in the previous diagrams.

### Build Method

1. Clone the repositories.

   ```bash
   git clone https://github.com/ypsitau/picopacman
   git clone https://github.com/ypsitau/picotetris
   git clone https://github.com/ypsitau/picoinvader
   git clone https://github.com/ypsitau/picohakomusu
   git clone https://github.com/ypsitau/picopegsol
   git clone https://github.com/ypsitau/pico-jxglib
   cd pico-jxglib
   git submodule update --init
   ```
