#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/SSD1306.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Shell.h"
// The directory jxglib/Font contains font headers generated from BDF font files.
// Here, we use naga10 which is a 10x10 pixel font.
#include "jxglib/Font/naga10.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    // Initialize USB host.
    USBHost::Initialize();
    // Create a USBHost::Keyboard instance to connect to a USB keyboard.
    USBHost::Keyboard keyboard;
    keyboard.SetCapsLockAsCtrl();
    // Initialize I2C0 for the display.
    ::i2c_init(i2c0, 400 * 1000);
    GPIO4.set_function_I2C0_SDA().pull_up();
    GPIO5.set_function_I2C0_SCL().pull_up();
    // Create a Display::SSD1306 instance with the specified I2C interface and address.
    Display::SSD1306 display(i2c0, 0x3c);
    // Initialize the display.
    display.Initialize();
    // Display::Terminal is a Terminal class that can be attached to a Display and a Keyboard.
    Display::Terminal terminal;
    // Initialize the terminal.
    terminal.Initialize();
    // Attach the display and keyboard to the terminal.
    terminal.AttachDisplay(display);
    terminal.AttachKeyboard(keyboard);
    // Set the font for the terminal.
    terminal.SetFont(Font::naga10);
    // Attach the terminal to the shell.
    Shell::AttachTerminal(terminal);
    for (;;) {
        Tickable::Tick();
    }
}
