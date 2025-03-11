//==============================================================================
// jxglib/Tickable.h
//==============================================================================
#ifndef PICO_JXGLIB_TICKABLE_H
#define PICO_JXGLIB_TICKABLE_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tickable
//------------------------------------------------------------------------------
class Tickable {
private:
	uint32_t msecTick_;
	uint32_t msecStart_;
	Tickable* pTickableNext_;
private:
	static Tickable* pTickableTop_;
	static bool firstFlag_;
	static uint32_t msecMainStart_;
public:
	Tickable(uint32_t msecTick = 0);
public:
	static bool Tick(uint32_t msecTick = 0);
	static void Sleep(uint32_t msecTick = 0) { while (!Tick(msecTick)) ::tight_loop_contents(); }
public:
	Tickable* GetNext() { return pTickableNext_; }
public:
	void DisableTick() { msecTick_ = -1; }
	void SetTick(uint32_t msecTick) { msecTick_ = msecTick; }
	uint32_t GetTick() const { return msecTick_; }
	void InitTick(uint32_t msecStart) { msecStart_ = msecStart; }
	bool IsExpired(uint32_t msecCur);
public:
	virtual void OnTick() = 0;
};

}

#endif
