#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/Display.h"
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
    Shell::Instance.Startup();
    for (;;) {
        Tickable::Tick();
    }
}
