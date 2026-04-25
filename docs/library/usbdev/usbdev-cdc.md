# Communcication Device Class (CDC)



## Single Serial Port

Here, we will create a USB device that implements the Communication Device Class (CDC) and echos back any data sent from the host computer.

### Building and Flashing the Program

Create a new Pico SDK project named `usbdev-cdc`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbdev-cdc/
    ├── CMakeLists.txt
    ├── usbdev-cdc.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/usbdev-cdc/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit `usbdev-cdc.cpp` as follows:

```cpp title="usbdev-cdc.cpp" linenums="1"
{% include "./sample/usbdev-cdc/usbdev-cdc.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

### Running the Program

## Multiple Serial Ports

Here, we will create a USB device that implements multiple CDC interfaces, which appear as multiple serial ports on the host computer. Each serial port will echo back data sent from the host in different manners such as transforming the characters to uppercase and encrypting them.

### Building and Flashing the Program

Create a new Pico SDK project named `usbdev-cdc-multi`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── usbdev-cdc-multi/
    ├── CMakeLists.txt
    ├── usbdev-cdc-multi.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/usbdev-cdc-multi/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

!!! note
    We set `CFG_TUD_CDC` to `3` in `jxglib_configure_USBDevice()` since we are using three CDC interfaces.

Edit `usbdev-cdc-multi.cpp` as follows:

```cpp title="usbdev-cdc-multi.cpp" linenums="1"
{% include "./sample/usbdev-cdc-multi/usbdev-cdc-multi.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

### Running the Program
