#include "pico/stdlib.h"
#include "jxglib/Tickable.h"

using namespace jxglib;

const GPIO& GPIO_LED1 = GPIO10;
const GPIO& GPIO_LED2 = GPIO11;
const GPIO& GPIO_LED3 = GPIO12;
const GPIO& GPIO_LED4 = GPIO13;
const GPIO& GPIO_ButtonResume = GPIO16;
const GPIO& GPIO_ButtonSuspend = GPIO17;

bool value_LED1 = false;
bool value_LED2 = false;
bool value_LED3 = false;
bool value_LED4 = false;

TickableEntry(BlinkLED1, 100)
{
	GPIO_LED1.put(value_LED1);
	value_LED1 = !value_LED1;
}

TickableEntry(BlinkLED2, 200)
{
	GPIO_LED2.put(value_LED2);
	value_LED2 = !value_LED2;
}

TickableEntry(BlinkLED3, 400)
{
	GPIO_LED3.put(value_LED3);
	value_LED3 = !value_LED3;
}

TickableEntry(BlinkLED4, 800)
{
	GPIO_LED4.put(value_LED4);
	value_LED4 = !value_LED4;
}

TickableEntry(Button, 20)
{
	if (!GPIO_ButtonResume.get()) ResumeTickable(BlinkLED1);
	if (!GPIO_ButtonSuspend.get()) SuspendTickable(BlinkLED1);
}

int main()
{
	GPIO_LED1.init().set_dir_OUT();
	GPIO_LED2.init().set_dir_OUT();
	GPIO_LED3.init().set_dir_OUT();
	GPIO_LED4.init().set_dir_OUT();
	GPIO_ButtonResume.init().set_dir_IN().pull_up();
	GPIO_ButtonSuspend.init().set_dir_IN().pull_up();
	for (;;) Tickable::Tick();
}
