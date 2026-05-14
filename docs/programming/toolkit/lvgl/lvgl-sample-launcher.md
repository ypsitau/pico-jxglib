# Sample Launcher

The directory `pico-jxglib/LVGL/lvgl/examples` contains over 100 LVGL sample programs. We have prepared Pico SDK projects to run these easily.

The same ILI9341 is used. The breadboard wiring image is the same. Connect a USB-serial converter to the UART port (TX: GPIO0, RX: GPIO1) or connect via USB to the PC and start a serial terminal app (communication speed 115200bps).

In the directory `pico-jxglib/LVGL/test-examples`, open VSCode and build the project. Run the program and you'll see a screen like this:

```text
--------
  1:anim_1                         52:style_5                       103:keyboard_2
  2:anim_2                         53:style_6                       104:label_1
  3:anim_3                         54:style_7                       105:label_2
 ...
 50:style_3                       101:imagebutton_1                 152:tileview_1
 51:style_4                       102:keyboard_1                    153:win_1
Enter Number:
```
