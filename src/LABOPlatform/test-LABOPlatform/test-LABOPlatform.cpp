#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main(void)
{
	::stdio_init_all();
	//LABOPlatform::Instance.AttachStdio().Initialize();
	LABOPlatform::Instance.Initialize();
	for (;;) Tickable::Tick();
}
