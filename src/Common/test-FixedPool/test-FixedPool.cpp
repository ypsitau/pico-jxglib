#include <stdio.h>
#include <malloc.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

FixedPool fixedPoolTest("fixedPoolTest", 32, 8);

class A {
private:
	char buff_[32];
public:
	FixedPoolAllocator(fixedPoolTest, "A")
public:
	A() {}
};

class B {
public:
	FixedPoolAllocator(fixedPoolTest, "B")
public:
	B() {}
};
	
class C {
private:
	char buff_[33];
public:
	FixedPoolAllocator(fixedPoolTest, "C")
public:
	C() {}
};
	
int main()
{
	::stdio_init_all();
#if 1
	fixedPoolTest.Initialize();
#endif
	FixedPool& pool = fixedPoolTest;
	::printf("----------------\n");
	for (int i = 0; i < 3; i++) {
		::printf("\n");
		do {
			auto p1 = new A;		pool.PrintUsage();
			delete p1;				pool.PrintUsage();
		} while (0);
		::printf("\n");
		do {
			auto p1 = new A;		pool.PrintUsage();
			delete p1;				pool.PrintUsage();
		} while (0);
		::printf("\n");
		do {
			auto p1 = new A;		pool.PrintUsage();
			auto p2 = new A;		pool.PrintUsage();
			delete p1;				pool.PrintUsage();
			delete p2;				pool.PrintUsage();
		} while (0);
		::printf("\n");
		do {
			auto p1 = new A;		pool.PrintUsage();
			auto p2 = new A;		pool.PrintUsage();
			auto p3 = new A;		pool.PrintUsage();
			delete p1;				pool.PrintUsage();
			delete p2;				pool.PrintUsage();
			delete p3;				pool.PrintUsage();
		} while (0);
		::printf("\n");
		do {
			auto p1 = new A;		pool.PrintUsage();
			auto p2 = new A;		pool.PrintUsage();
			auto p3 = new A;		pool.PrintUsage();
			delete p1;				pool.PrintUsage();
			delete p2;				pool.PrintUsage();
			auto p4 = new B;		pool.PrintUsage();
			delete p3;				pool.PrintUsage();
			delete p4;				pool.PrintUsage();
		} while (0);
		::printf("\n");
		do {
			auto p1 = new A;		pool.PrintUsage();
			auto p2 = new A;		pool.PrintUsage();
			auto p3 = new A;		pool.PrintUsage();
			auto p4 = new A;		pool.PrintUsage();
			auto p5 = new A;		pool.PrintUsage();
			auto p6 = new A;		pool.PrintUsage();
			auto p7 = new A;		pool.PrintUsage();
			auto p8 = new A;		pool.PrintUsage();
			delete p4;				pool.PrintUsage();
			delete p3;				pool.PrintUsage();
			delete p1;				pool.PrintUsage();
			delete p2;				pool.PrintUsage();
			auto p9 = new B;		pool.PrintUsage();
			auto p10 = new B;		pool.PrintUsage();
			delete p8;				pool.PrintUsage();
			delete p7;				pool.PrintUsage();
			delete p5;				pool.PrintUsage();
			delete p6;				pool.PrintUsage();
			delete p9;				pool.PrintUsage();
			delete p10;				pool.PrintUsage();
		} while (0);
	}
#if 0
	::printf("\n");
	do {
		auto p1 = new A;			pool.PrintUsage();
		auto p2 = new A;			pool.PrintUsage();
		auto p3 = new A;			pool.PrintUsage();
		auto p4 = new A;			pool.PrintUsage();
		auto p5 = new A;			pool.PrintUsage();
		auto p6 = new A;			pool.PrintUsage();
		auto p7 = new A;			pool.PrintUsage();
		auto p8 = new A;			pool.PrintUsage();
		auto p9 = new A;			pool.PrintUsage();
	} while (0);
#endif
#if 0
	::printf("\n");
	do {
		auto p = new C;				pool.PrintUsage();
	} while (0);
#endif
	for (;;) tight_loop_contents();
}
