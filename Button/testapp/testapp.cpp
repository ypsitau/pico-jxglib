#include <stdio.h>
#include "jxglib/Button.h"

using namespace jxglib;

//Button buttonLeft(10, "Left"), buttonUp(11, "Up");
//Button buttonDown(12, "Down"), buttonRight(13, "Right");
//Button buttonA(14, "A"), buttonB(15, "B");

int main()
{
	Button buttonLeft(GPIO10, "Left"), buttonUp(GPIO11, "Up");
	Button buttonDown(GPIO12, "Down"), buttonRight(GPIO13, "Right");
	Button buttonA(GPIO14, "A"), buttonB(GPIO15, "B");
	::stdio_init_all();
	Button::Initialize(50, true);	// polling time: 50msec
	int n = 0;
	for (;;) {
		Button::Event event = Button::ReadEvent();
		if (!event.IsNone()) {
			printf("%-8s %s\n", event.IsPressed()? "Pressed" : "Released", event.GetName());
			//if (event.IsPressed(buttonLeft)) {
			//	n++;
			//	printf("count: %d\n", n);
			//}
		}
	}
}
