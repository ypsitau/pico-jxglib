#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"

using namespace jxglib;

Serial::Terminal terminal;

GPIO::Key keyA(GPIO16, 'Z', GPIO::Key::PullUp);
GPIO::Key keyB(GPIO17, 'X', GPIO::Key::PullUp);
GPIO::Key keyLeft(GPIO18, VK_LEFT, GPIO::Key::PullUp);
GPIO::Key keyUp(GPIO19, VK_UP, GPIO::Key::PullUp);
GPIO::Key keyDown(GPIO20, VK_DOWN, GPIO::Key::PullUp);
GPIO::Key keyRight(GPIO21, VK_RIGHT, GPIO::Key::PullUp);

GPIO::Keyboard keyboard;

TickableEntry(hoge, 300)
{
	static bool value = false;
	value = !value;
	GPIO2.put(value);
}

int main()
{
	::stdio_init_all();
	GPIO2.init().set_dir_OUT();
	keyboard.Initialize();
	terminal.Initialize();
	//terminal.AttachInput(keyboard);
	terminal.AttachInput(Stdio::Keyboard::Instance);
	terminal.AttachOutput(Stdio::Printable::Instance);
	terminal.Println("ReadLine Test Program");
	for (;;) {
		::printf("%s\n", terminal.ReadLine(">"));
		//::printf("%d\n", terminal.GetChar());
	}
}
