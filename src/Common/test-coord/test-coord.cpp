#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

void test_Rect()
{
	Point pt;
	Rect rect(30, 50, 120, 130);
	Rect rectMod;
	printf("Rect = (%d, %d, %d, %d)\n", rect.x, rect.y, rect.width, rect.height);
	pt = rect.GetPointN();			::printf("GetPointN       %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointW();			::printf("GetPointW       %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointE();			::printf("GetPointE       %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointS();			::printf("GetPointS       %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointNW();			::printf("GetPointNW      %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointNE();			::printf("GetPointNE      %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointSW();			::printf("GetPointSW      %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointSE();			::printf("GetPointSE      %d, %d\n", pt.x, pt.y);
	pt = rect.GetPointCenter();		::printf("GetPointCenter  %d, %d\n", pt.x, pt.y);
	rectMod = rect.Inflate(1);		::printf("Inflate(1)      %d, %d, %d, %d\n", rectMod.x, rectMod.y, rectMod.width, rectMod.height);
	rectMod = rect.Inflate(5);		::printf("Inflate(5)      %d, %d, %d, %d\n", rectMod.x, rectMod.y, rectMod.width, rectMod.height);
	rectMod = rect.Inflate(-1);		::printf("Inflate(-1)     %d, %d, %d, %d\n", rectMod.x, rectMod.y, rectMod.width, rectMod.height);
	rectMod = rect.Inflate(-5);		::printf("Inflate(-5)     %d, %d, %d, %d\n", rectMod.x, rectMod.y, rectMod.width, rectMod.height);
	rectMod = rect.Inflate(-62);	::printf("Inflate(-62)    %d, %d, %d, %d\n", rectMod.x, rectMod.y, rectMod.width, rectMod.height);
	rectMod = rect.Inflate(-67);	::printf("Inflate(-67)    %d, %d, %d, %d\n", rectMod.x, rectMod.y, rectMod.width, rectMod.height);
}

int main()
{
	::stdio_init_all();
	test_Rect();
}
