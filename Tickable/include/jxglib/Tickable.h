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
public:
	class Master {
	private:
		Tickable* pTickableTop_;
		bool firstFlag_;
	public:
		static Master Instance;
	public:
		Master() : pTickableTop_{nullptr}, firstFlag_{true} {}
	public:
		void AddTickable(Tickable* pTickable);
		void Tick();
	};
private:
	uint32_t msecTick_;
	uint32_t msecStart_;
	Tickable* pTickableNext_;
public:
	Tickable(uint32_t msecTick = -1) : msecTick_{msecTick}, msecStart_{0}, pTickableNext_{nullptr} {}
public:
	static void AddTickable(Tickable* pTickable) { Master::Instance.AddTickable(pTickable); }
	static void Tick() { Master::Instance.Tick(); }
public:
	void SetNext(Tickable* pTickable) { pTickableNext_ = pTickable; }
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
