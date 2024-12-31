//==============================================================================
// jxglib/Pointer.h
//==============================================================================
#ifndef PICO_JXGLIB_POINTER_H
#define PICO_JXGLIB_POINTER_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Pointer
//------------------------------------------------------------------------------
template<typename T> class Pointer {
protected:
	T p_;
public:
	Pointer(T p) : p_{p} {}
public:
	char Get() const { return *p_; }
	T GetPointer() const { return p_; }
public:
	virtual Pointer& Forward() { p_++; return *this; }
	virtual Pointer& Backward() { p_--; return *this; }
};

//------------------------------------------------------------------------------
// Pointer_Round
//------------------------------------------------------------------------------
template<typename T> class Pointer_Round : public Pointer<T> {
private:
	T pBegin_;
	T pEnd_;
	T p_;
public:
	Pointer_Round(T p, T pBegin, T pEnd) : Pointer<T>(p), pBegin_{pBegin}, pEnd_{pEnd} {}
public:
	virtual Pointer<T>& Forward() override { p_ = (p_ + 1 == pEnd_)? pBegin_ : p_ + 1; return *this; }
	virtual Pointer<T>& Backward() override { p_ = (p_ == pBegin_)? pEnd_ - 1 : p_ - 1; return *this; }
};

//------------------------------------------------------------------------------
// CharFeeder
//------------------------------------------------------------------------------
using CharFeeder = Pointer<const char*>;

//------------------------------------------------------------------------------
// CharFeeder_Round
//------------------------------------------------------------------------------
using CharFeeder_Round = Pointer_Round<const char*>;

}

#endif
