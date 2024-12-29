//==============================================================================
// jxglib/CharFeeder.h
//==============================================================================
#ifndef PICO_JXGLIB_CHARFEEDER_H
#define PICO_JXGLIB_CHARFEEDER_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// CharFeeder
//------------------------------------------------------------------------------
class CharFeeder {
public:
	virtual char Next() = 0;
};

//------------------------------------------------------------------------------
// CharFeeder_Forward
//------------------------------------------------------------------------------
class CharFeeder_Forward : public CharFeeder {
private:
	const char* p_;
public:
	CharFeeder_Forward(const char* p) : p_{p} {}
public:
	virtual char Next() override { return *p_++; }
};

//------------------------------------------------------------------------------
// CharFeeder_Round
//------------------------------------------------------------------------------
class CharFeeder_Round : public CharFeeder {
private:
	const char* pTop_;
	const char* pBottom_;
	const char* p_;
public:
	CharFeeder_Round(const char* pTop, const char* pBottom, const char* p) : pTop_{pTop}, pBottom_{pBottom}, p_{p} {}
public:
	virtual char Next() override {
		if (p_ == pBottom_) p_ = pTop_;
		return *p_++;
	}
};

}

#endif
