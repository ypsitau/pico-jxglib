#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Initialize the USB Host and Keyboard
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    for (;;) {
        KeyData keyData;
        if (keyboard.GetKeyDataNB(&keyData)) {
            char ch = keyData.GetChar();
            if (ch) {
                ::printf("Char:    '%c'\n", ch);
            } else {
                ::printf("KeyCode: 0x%02x (modifier: %03b)\n", keyData.GetKeyCode(), keyData.GetModifier());
            }
        }
        Tickable::Sleep(100);
    }
}
