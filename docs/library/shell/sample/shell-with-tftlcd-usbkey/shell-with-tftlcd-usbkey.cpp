#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Shell.h"
// The directory jxglib/Font contains font headers generated from BDF font files.
// Here, we use shinonome16 which is a 16x16 pixel font.
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
    // Initialize USB host.
	USBHost::Initialize();
    // Create a USBHost::Keyboard instance to connect to a USB keyboard.
	USBHost::Keyboard keyboard;
    keyboard.SetCapsLockAsCtrl();
    // Initialize SPI1 for the display.
	::spi_init(spi1, 125'000'000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
    // Create a Display::ST7789 instance with the specified SPI interface,
    // the display resolution, and pin configuration.
	Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    // Initialize the display.
    display.Initialize(Display::Dir::Rotate90);
    // Display::Terminal is a Terminal class that can be attached to a Display and a Keyboard.
	Display::Terminal terminal;
    // Initialize the terminal.
	terminal.Initialize();
    // Attach the display and keyboard to the terminal.
    terminal.AttachDisplay(display);
	terminal.AttachKeyboard(keyboard);
    // Set the font for the terminal.
    terminal.SetFont(Font::shinonome16);
    // Attach the terminal to the shell.
	Shell::AttachTerminal(terminal);
	for (;;) {
        Tickable::Tick();
    }
}
