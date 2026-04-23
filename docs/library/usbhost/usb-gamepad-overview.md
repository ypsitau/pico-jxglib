Previously, I published an article titled ["Connecting USB Keyboards and Mice to the Pico Board with pico-jxglib"](https://zenn.dev/ypsitau/articles/2025-04-02-usbhost-keyboard-mouse). This time, we'll use the Pico's USB host functionality to connect a USB gamepad to the Pico board—and enjoy some games as well.

## About USB Gamepads

As the name suggests, gamepads are devices primarily designed for gaming, but they are also useful as manipulators for controlling robots. They typically feature two analog joysticks (left and right), a D-pad, and more than 10 buttons, all housed in a body that allows efficient operation with both hands. The fact that such devices are available for just a few thousand yen is very appealing.

However, since many manufacturers have released products with various specifications, the standards are quite chaotic. To help you choose a suitable product, here is a brief summary of gamepad specifications.

### DirectInput and XInput

There are two main interface standards for USB gamepads: **DirectInput** and **XInput**. **pico-jxglib** supports **DirectInput**.

**DirectInput** is an old standard implemented as a USB HID class. The HID class is designed to support various human interface devices such as keyboards, mice, buttons, analog joysticks, touch panels, and pointing devices. It uses a specification called the Report Descriptor, which allows the device to define its own data format. This flexibility enables manufacturers to freely arrange the number and layout of buttons and controls, but it also leads to disorder. Not only do the bit layouts in the report data differ between products, but the interpretation of Usage values (which define the purpose of buttons, etc., in the Report Descriptor) also varies by manufacturer—and sometimes even between products from the same manufacturer. The multimedia library SDL for PCs provides a gamepad API, but it absorbs these differences using a [device-specific database](https://github.com/mdqinc/SDL_GameControllerDB).

**XInput** is a gamepad-specific interface proposed by Microsoft during the development of the Xbox game console. Perhaps to avoid the disorder of DirectInput, XInput strictly defines the number and layout of buttons and joysticks. However, it is not a HID class; instead, it is treated as a vendor-specific class with Vendor ID `0x045E` (Microsoft) and Product ID `0x028E` (Xbox 360 Controller), among others. Because it is a vendor-specific class, third-party manufacturers who want to support XInput must install their own Vendor ID and Product ID into the OS using INF files, etc.[^vendorid]

[^vendorid]: Some gamepads use Microsoft's Vendor ID and Product ID for their own products without permission. This may be to allow easy connection without installation, but it is essentially device spoofing and could be problematic.

Many third-party gamepads can switch between DirectInput and XInput modes. However, gamepads marketed as Xbox controllers naturally support only XInput.

### USB Gamepads Compatible with the Pico Board

Out of all commercially available USB gamepads, only a limited number can be connected to the Pico board. I obtained three gamepads from different vendors, but only one of them worked.

- The EasySMX wireless ESM-9013 was not recognized even when the receiver was connected to the USB port. It was recognized if the board was reset while connected, but it remained unstable when connected via a USB hub. Once connected successfully, report reception worked normally.

- The Logicool wired F310 is a very popular product, but I have not been able to use it so far. There are no hardware connection issues, and the descriptor can be retrieved, but reports cannot be received. There were rare occasions when reports were received, so I believe this can be resolved by fixing tinyusb, but the cause is unknown.

- **The Elecom wireless JC-U4113S** is the only gamepad among those I have that works reliably.

## About the Report Descriptor Parser

USB HID class devices periodically send data called reports to the host, and the format of this data is defined by the Report Descriptor. The Report Descriptor provides information such as the position of keyboard, button, and joystick data within the report, their maximum and minimum values, and their intended usage.

To support various data types, the format specification is somewhat complex. When obtaining information from a USB gamepad, I was hesitant to create a full-fledged parser just to handle a few dozen bytes of data. If you only want to support a specific USB gamepad, you can simply hard-code the structure. In fact, the tinyusb sample for gamepads is implemented this way.

However, I couldn't resist the urge to implement a parser that can extract all information from the Report Descriptor. This should allow support for various gamepads, and since it would be a waste not to use it for other purposes, I plan to write a separate article detailing it.
