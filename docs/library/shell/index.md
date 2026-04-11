# Shell

The shell can work with a variety of devices.

For input devices, it supports:

```mermaid
block
columns 1
  block:Input
    A("USB Serial")
    B("UART")
    C("Wi-Fi (Telnet)")
    D("USB HID Keyboard")
    E("GPIO Buttons")
    F("GPIO Key Matrix")
  end
```

For output devices, it supports:

```mermaid
block
columns 1
  block:Output
    E("USB Serial")
    F("UART")
    G("Wi-Fi (Telnet)")
    H("OLED Display")
    I("TFT LCD Display")
  end
```

You can easily create shell commands. By simply linking the source or library that implements the command, it becomes available from the shell, making embedding and removal easy.
