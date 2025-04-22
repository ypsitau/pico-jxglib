#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"
#include "jxglib/KeyboardTest.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	USBHost::Keyboard keyboard[2];
	//KeyboardTest::GetKeyDataNB(Stdio::Instance, keyboard[0]);
	//KeyboardTest::SenseKeyData(Stdio::Instance, keyboard);
	//KeyboardTest::SenseKeyCode(Stdio::Instance, keyboard);
	//KeyboardTest::IsPressed(Stdio::Instance, keyboard);
	for (;;) {
		uint8_t keyCodeTbl0[8];
		uint8_t keyCodeTbl1[8];
		int nKeys0 = keyboard[0].SenseKeyCode(keyCodeTbl0, count_of(keyCodeTbl0));
		int nKeys1 = keyboard[1].SenseKeyCode(keyCodeTbl1, count_of(keyCodeTbl1));
		::printf("%02x %02x\n", nKeys0? keyCodeTbl0[0] : 0, nKeys1? keyCodeTbl1[0] : 0);
		Tickable::Sleep(100);
	}
}
