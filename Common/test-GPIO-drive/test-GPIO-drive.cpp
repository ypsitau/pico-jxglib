#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	GPIO10.init().set_dir(true).set_drive_strength_2MA().put(true);
	GPIO11.init().set_dir(true).set_drive_strength_4MA().put(true);
	GPIO12.init().set_dir(true).set_drive_strength_8MA().put(true);
	GPIO13.init().set_dir(true).set_drive_strength_12MA().put(true);
}
