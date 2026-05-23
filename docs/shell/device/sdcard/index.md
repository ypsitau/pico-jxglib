# SD Card

In this page, we connect an SD card to the Pico board by using shell commands.

## SD Card Reader Modules

SD card reader modules are basically just an SD card slot with some resistors and a voltage regulator, but the supply voltage, signal level, and presence of pull-up resistors vary by module, so be careful.

Below are some SD card reader modules I had on hand (mainly from Amazon), with notes on supply voltage, pull-up resistors, and signal levels.

|Appearance|Notes|
|----|----|
|![sdcard-adapter](images/sdcard-adapter.jpg)|Standard SD card module. Both **5V** and **3.3V** supply pins are available. If using 5V, a voltage regulator steps down to 3.3V for the SD card. All signal lines have 10kΩ pull-up resistors, so **no external pull-ups are needed**. Signal level is **3.3V**.|
|![ILI9341-back](images/ILI9341-back.jpg)|Standard SD card slot on the TFT LCD ILI9341. Power is supplied from the TFT LCD connector at **3.3V**. **External pull-up resistors are required**[^pullup]. Signal level is **3.3V**.|
|![u-sdcard-adapter-1](images/u-sdcard-adapter-1.jpg)|microSD card module. Supply voltage is **5V**, stepped down to 3.3V by a voltage regulator. **No external pull-ups are needed**. Signal lines have a buffer (74HC125), so it can connect to both **3.3V** and **5V** signal levels.|
|![u-sdcard-adapter-2](images/u-sdcard-adapter-2.jpg)|microSD card module. Supply voltage is **3.3V**. All signal lines have 10kΩ pull-up resistors, so **no external pull-ups are needed**. Signal level is **3.3V**.|

To distinguish the supply voltage, if the SD card reader module has a voltage regulator like the one shown below, use 5V supply; if not, use 3.3V supply.

![sdcard-adapter-voltage](images/sdcard-adapter-voltage.jpg)

[^pullup]: In my tests, the SD card worked without pull-up resistors, but some SD cards may require them. If it doesn't work, check for pull-up resistors.
