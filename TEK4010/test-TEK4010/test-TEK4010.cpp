#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TEK4010.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	TEK4010 tek(UART::Default);
	tek.ClearScreen();
	tek.Printf("hello\n");
	for (int i = 0; i < 8; i++) {
		tek.SetColor(Color::red);
		tek.SetColorBg(Color::green);
		tek.Printf("hello\n");
	}
	for (uint16_t i = 0; i < 480; i += 10) {
		tek.DrawLine(0, 240, 600, i, 0);
	}
	tek.MoveTo(420, 20);
	tek.TextMode();
	tek.Printf("Hello World");
}
