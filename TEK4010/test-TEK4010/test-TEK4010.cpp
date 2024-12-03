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
	tek.SendCtrlShift('M');
	tek.SendCoord(512, 320);
	tek.SendCoord(0, 0);
	tek.SendCtrlShift('M');
	tek.SendCoord(20, 30);
	tek.SendCoord(200, 30);
	//tek.SendString(",``0@");
	//tek.SendChar(',');
	//tek.SendChar('@');
	//tek.SendChar(',');
	//tek.SendCoord(300, 300);
	//tek.SendChar('@');
	//tek.SendString(",``0@");
	//tek.SendChar('@');	
	//tek.SendString(",o`0@");
	//tek.SendChar('@');	
	//tek.SendCoord(512, 390);
	//tek.SendChar(',');
	//tek.SendCoord(300, 300);
	//tek.SendChar(',');
	//tek.SendCoord(301, 300);
	
	//tek.SendString("f @f0@");
	//tek.SendString(" ` @");
	
	//tek.EnterGraphMode();
	//tek.MoveTo(100, 300);
	//tek.LineTo(0, 0);
	//tek.SendCoord(300, 50);
	//tek.SendCoord(100, 50);
	//tek.MoveTo(600, 600);
	//tek.LineTo(600, 100);
	//tek.DrawLine(0, 0, 600, 600, tek.LineStyle::Solid);
	//tek.DrawLine(0, 600, 600, 600, tek.LineStyle::Solid);
	//tek.EnterGraphMode();
	//tek.DrawLine(0, 0, 600, 600, 0);
	//tek.MoveTo(300, 300);
	//tek.EnterGraphMode();
	//tek.PutChar('@');
#if 0
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
#endif
}
