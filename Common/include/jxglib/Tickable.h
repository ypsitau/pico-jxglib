//==============================================================================
// jxglib/Tickable.h
//==============================================================================
#ifndef PICO_JXGLIB_TICKABLE_H
#define PICO_JXGLIB_TICKABLE_H
#include "pico/stdlib.h"

namespace jxglib {

class Printable;

#define TickableEntry(name, ...) \
class Tickable_##name##_ : public Tickable { \
public: \
	static Tickable_##name##_ Instance; \
public: \
	Tickable_##name##_() : Tickable(__VA_ARGS__) {} \
	virtual const char* GetTickableName() const override { return #name; } \
	virtual void OnTick() override; \
}; \
Tickable_##name##_ Tickable_##name##_::Instance; \
void Tickable_##name##_::OnTick()

#define SetTickableTick(name, msecTick) Tickable_##name##_::Instance.SetTick(msecTick)
#define GetTickableTick(name) Tickable_##name##_::Instance.GetTick()
#define ResumeTickable(name) Tickable_##name##_::Instance.Resume()
#define SuspendTickable(name) Tickable_##name##_::Instance.Suspend()

//------------------------------------------------------------------------------
// Tickable
//------------------------------------------------------------------------------
class Tickable {
public:
	enum class Priority { Lowest, BelowNormal, Normal, AboveNormal, Highest };
private:
	Priority priority_;
	uint32_t msecTick_;
	uint32_t msecStart_;
	bool runningFlag_;
	Tickable* pTickableNext_;
private:
	static Tickable* pTickableTop_;
	static bool firstFlag_;
	static uint32_t msecMainStart_;
public:
	Tickable(uint32_t msecTick = 0, Priority priority = Priority::Normal);
public:
	static bool Tick(uint32_t msecTick = 0);
	static void Sleep(uint32_t msecTick = 0) { while (!Tick(msecTick)) ::tight_loop_contents(); }
	static uint32_t GetCurrentTime() { return ::to_ms_since_boot(::get_absolute_time()); }
public:
	Priority GetPriority() const { return priority_; }
	const char* GetPriorityName() const { return GetPriorityName(GetPriority()); }
	Tickable* GetNext() { return pTickableNext_; }
	const Tickable* GetNext() const { return pTickableNext_; }
	void SetNext(Tickable* pTickable) { pTickableNext_ = pTickable; }
public:
	bool IsRunning() const { return runningFlag_; }
	void Resume() { runningFlag_ = true; }
	void Suspend() { runningFlag_ = false; }
	void SetTick(uint32_t msecTick) { msecTick_ = msecTick; }
	uint32_t GetTick() const { return msecTick_; }
	void InitTick(uint32_t msecStart) { msecStart_ = msecStart; }
	bool IsExpired(uint32_t msecCur);
	void ResetTick(uint32_t msecTick);
public:
	virtual const char* GetTickableName() const { return "no-name"; }
	virtual void OnTick() = 0;
public:
	static void PrintList(Printable& printable);
	static const char* GetPriorityName(Priority priority);
};

}

#endif
