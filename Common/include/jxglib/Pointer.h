//==============================================================================
// jxglib/Pointer.h
//==============================================================================
#ifndef PICO_JXGLIB_POINTER_H
#define PICO_JXGLIB_POINTER_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PointerWrapped
//------------------------------------------------------------------------------
template<typename T> class PointerWrapped {
private:
	T p_;
	T pBegin_;
	T pEnd_;
public:
	PointerWrapped(T p, T pBegin, T pEnd) : p_{p}, pBegin_{pBegin}, pEnd_{pEnd} {}
public:
	char Get() const { return *p_; }
	T GetPointer() const { return p_; }
	PointerWrapped& Forward() {
		p_ = (p_ + 1 == pEnd_)? pBegin_ : p_ + 1;
		return *this;
	}
	PointerWrapped& Backward() {
		p_ = (p_ == pBegin_)? pEnd_ - 1 : p_ - 1;
		return *this;
	}
	int GetOffset() const { return p_ - pBegin_; }
};

//------------------------------------------------------------------------------
// CharFeederWrapped
//------------------------------------------------------------------------------
using CharFeederWrapped = PointerWrapped<const char*>;

}

#endif
