#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LineBuff.h"

using namespace jxglib;

void Println(CharFeederWrapped&& charFeeder)
{
	for (;; charFeeder.Forward()) {
		char ch = charFeeder.Get();
		if (!ch) break;
		::putchar(ch);
	}
	::putchar('\n');
}

void PrintForward(LineBuff& lineBuff)
{
	::printf("[Forward]\n");
	const char* pLine = lineBuff.GetLineFirst();
	do {
		Println(lineBuff.MakeCharFeeder(pLine));
	} while (lineBuff.NextLine(&pLine));
}

void PrintBackward(LineBuff& lineBuff)
{
	::printf("[Backward]\n");
	const char* pLine = lineBuff.GetLineLast();
	do {
		Println(lineBuff.MakeCharFeeder(pLine));
	} while (lineBuff.PrevLine(&pLine));
}

int main()
{
	::stdio_init_all();
	::printf("----\n");
	LineBuff lineBuff;
	lineBuff.Allocate(64);
	lineBuff.MarkLineLast().PutString("*ABCDEF").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*GHIJKLMN").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*OPQRST").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*UVWXYZ").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*ABCDE").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*FGHIJKL").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*MNOPQRSTU").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*VWXYZ").PutChar('\0');
	lineBuff.Print();
	PrintForward(lineBuff);
	PrintBackward(lineBuff);
	lineBuff.MarkLineLast().PutString("*ABCDE").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*FGHIJKL").PutChar('\0');
	lineBuff.Print();
	PrintForward(lineBuff);
	PrintBackward(lineBuff);
	lineBuff.MarkLineLast().PutString("*MN").PutChar('\0');
	lineBuff.MarkLineLast().PutString("*OPQRSTU").PutChar('\0');
	lineBuff.Print();
	PrintForward(lineBuff);
	PrintBackward(lineBuff);
}
