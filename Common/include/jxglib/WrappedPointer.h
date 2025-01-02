//==============================================================================
// jxglib/WrappedPointer.h
//==============================================================================
#ifndef PICO_JXGLIB_WRAPPEDPOINTER_H
#define PICO_JXGLIB_WRAPPEDPOINTER_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// WrappedPointer
//------------------------------------------------------------------------------
template<typename T> class WrappedPointer {
private:
	T p_;
	T pBegin_;
	T pEnd_;
public:
	WrappedPointer(T p, T pBegin, T pEnd) : p_{p}, pBegin_{pBegin}, pEnd_{pEnd} {}
	WrappedPointer(const WrappedPointer& wrappedPointer) :
		WrappedPointer(wrappedPointer.p_, wrappedPointer.pBegin_, wrappedPointer.pEnd_) {}
public:
	char Get() const { return *p_; }
	T GetPointer() const { return p_; }
	WrappedPointer& Forward() {
		p_ = (p_ + 1 == pEnd_)? pBegin_ : p_ + 1;
		return *this;
	}
	WrappedPointer& Backward() {
		p_ = (p_ == pBegin_)? pEnd_ - 1 : p_ - 1;
		return *this;
	}
	int GetOffset() const { return p_ - pBegin_; }
};

//------------------------------------------------------------------------------
// WrappedCharFeeder
//------------------------------------------------------------------------------
using WrappedCharFeeder = WrappedPointer<const char*>;

}

#endif
