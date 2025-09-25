//==============================================================================
// jxglib/PeriodicToggle.h
//==============================================================================
#ifndef PICO_JXGLIB_PERIODICTOGGLE_H
#define PICO_JXGLIB_PERIODICTOGGLE_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PeriodicToggle
//------------------------------------------------------------------------------
template<void (*Initialize)(), void (*PutOutput)(bool)>
class PeriodicToggle : public Tickable {
private:
    bool enableFlag_;
	bool value_;
	uint32_t msecStart_;
	uint32_t msecOn_;
	uint32_t msecOff_;
public:
	PeriodicToggle() : Tickable(0, Priority::Lowest),
            enableFlag_{false}, value_{false}, msecStart_{0}, msecOn_{500}, msecOff_{500} {}
public:
	void SetDurations(uint32_t msecOn, uint32_t msecOff) { msecOn_ = msecOn; msecOff_ = msecOff; }
	void Enable(bool enableFlag);
public:
    // virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "PeriodicToggle"; }
	virtual void OnTick() override;
};

template<void (*Initialize)(), void (*PutOutput)(bool)>
void PeriodicToggle<Initialize, PutOutput>::Enable(bool enableFlag)
{
    value_ = enableFlag_ = enableFlag;
	msecStart_ = Tickable::GetCurrentTimeSaved();
	PutOutput(value_);
}

template<void (*Initialize)(), void (*PutOutput)(bool)>
void PeriodicToggle<Initialize, PutOutput>::OnTick()
{
	if (IsFirstTick()) Initialize();
    if (!enableFlag_) return;
	uint32_t msecCur = Tickable::GetCurrentTimeSaved();
	if (msecCur - msecStart_ < (value_? msecOn_ : msecOff_)) return;
	value_ = !value_;
	PutOutput(value_);
	msecStart_ = msecCur;
}

}

#endif
