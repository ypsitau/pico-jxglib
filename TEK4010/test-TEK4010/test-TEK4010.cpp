#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/TEK4010.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	TEK4010 tek(UART::Default);
	tek.OpenWindow();
	tek.ClearScreen();

	tek.SetColor(Color::red);
	for (int i = 0; i < 8; i++) {
		//tek.SetColorBg(Color::green);
		tek.Printf("hello %d\n", i);
	}
	for (uint16_t i = 0; i < 480; i += 10) {
		tek.DrawLine(0, 240, 1023, i, 0);
	}
	for (uint16_t i = 0; i < 480; i += 10) {
		tek.MoveTo(420, i);
		tek.Printf("Hello World");
	}
	tek.EnterGraphMode();
	tek.EnterPlotMode();
	tek.Print("PPPPPPPPPPPPPPPPPPPPPPPPPPP            PPPPPPPPPPPPPPPPPPP");
	tek.CloseWindow();
}
