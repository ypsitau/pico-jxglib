# USB Device

Pico SDK is shipped with TinyUSB, a cross-platform USB stack. Thanks to this library, you can use the USB peripheral on the RP2040 to create a custom USB device, such as a keyboard, mouse, or mass storage device. However, it requires a good understanding of the USB protocol especially about USB descriptors. Device descriptors, configuration descriptors, interface descriptors, endpoint descriptors, and string descriptors... Although thse descriptors are essential for USB communication, they can be quite complex to implement correctly.

pico-jxglib provides a higher-level API that abstracts away the complexities of USB descriptors, making it easier to create custom USB devices. With this library, you can define your USB device's functionality using simple C++ code, without needing to manually craft USB descriptors.

## Example: CDC EchoBack

To see how to use pico-jxglib to create a custom USB device, let's look at an example of a Communications Device Class that echos back data.

Below is the additional code to your `CMakeLists.txt` file:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/usbdev-cdc/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

- **Line 2** `jxglib_USBDevice` is the library that handles various USB descriptors.
- **Line 3** `jxglib_configure_USBDevice()` generates a configuration file, `tusb_config.h`, that is required by TinyUSB. Following arguments specify the number of USB interfaces used by the device:
    - `CFG_TUD_CDC`: CDC interface
    - `CFG_TUD_HID`: Human Interface Device interface
    - `CFG_TUD_MSC`: Mass Storage Class interface
    - `CFG_TUD_VIDEO`: Video Class interface

Below is the complete code for `usbdev-cdc.cpp`:

```cpp title="usbdev-cdc.cpp" linenums="1"
{% include "./sample/usbdev-cdc/usbdev-cdc.cpp" %}
```

- **Line 9**: `USBDevice::Controller` is the main class that manages the USB descriptor.
- **Line 10-17**: These fields are used to define the USB Device descriptor.
- **Line 19**: Language Id used in string descriptors. `0x0409` is the language ID for English (United States).
- **Line 20-22**: Specify the manufacture, product, and serial number string descriptors.
- **Line 24**: Creates an instance of USBDevice::CDC, one of the USB interfaces. Although each interface class has different arguments, it generally requires a string for the interface name and addresses for IN and OUT endpoints. You can create multiple instances of interface classes by assigning different endpoint addresses.
