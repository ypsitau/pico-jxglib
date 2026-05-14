#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/Display/ST7735.h"
#include "jxglib/DrawableTest.h"

using namespace jxglib;

ShellCmd_Named(draw_rotate, "draw-rotate", "Rotate the image")
{
    auto& display = Display::GetInstance(0);
    DrawableTest::RotateImage(display);
    return Result::Success;
}

ShellCmd_Named(draw_string, "draw-string", "Draw a string")
{
    auto& display = Display::GetInstance(0);
    DrawableTest::DrawString(display);
    return Result::Success;
}

ShellCmd_Named(draw_fonts, "draw-fonts", "Draw fonts")
{
    auto& display = Display::GetInstance(0);
    DrawableTest::DrawFonts(display);
    return Result::Success;
}

int main()
{
    ::stdio_init_all();
    // Prepare the Shell with Stdio
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    // Initialize the display
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    Display::ST7735 display(spi1, 80, 160, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate0);
    for (;;) {
        Tickable::Tick();
    }
}
