#include <stdio.h>
#include "jxglib/Button.h"

using namespace jxglib;

Button buttonLeft(GPIO10, "Left"), buttonUp(GPIO11, "Up");
Button buttonDown(GPIO12, "Down"), buttonRight(GPIO13, "Right");
Button buttonA(GPIO14, "A"), buttonB(GPIO15, "B");

int main()
{
	::stdio_init_all();
	Button::Initialize(50, true);	// polling time: 50msec
	int n = 0;
	for (;;) {
		Button::Event event = Button::ReadEvent();
		if (!event.IsNone()) {
			printf("%-8s %s\n", event.IsPressed()? "Pressed" : "Released", event.GetName());
			if (event.IsPressed()) printf("%s\n", event.GetName());
			if (event.IsPressed(buttonLeft)) {
				n++;
				printf("count: %d\n", n);
			}
		}
	}
}
