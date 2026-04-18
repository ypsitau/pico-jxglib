## Keyboard Classes

```mermaid
classDiagram
  class Keyboard {
    GetKeyCode()
    GetKeyData()
    GetKeyCodeNB()
    GetKeyDataNB()
  }
  class USBHost_Keyboard["USBHost::Keyboard"] {
  }
  class GPIO_Keyboard["GPIO::Keyboard"] {
  }
  class GPIO_KeyboardMatrix["GPIO::KeyboardMatrix"] {
  }
  class VT100_Keyboard["VT100::Keyboard"] {
  }
  Keyboard <|-- USBHost_Keyboard
  Keyboard <|-- GPIO_Keyboard
  Keyboard <|-- GPIO_KeyboardMatrix
  Keyboard <|-- VT100_Keyboard
```

## Virtual Keycode

[https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
