//==============================================================================
// jxglib/PeriodicToggle.h
//==============================================================================
#ifndef PICO_JXGLIB_PERIODICTOGGLE_H
#define PICO_JXGLIB_PERIODICTOGGLE_H
#include <memory.h>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// PeriodicToggle
//------------------------------------------------------------------------------
template<void (*Initialize)(), void (*PutOutput)(bool)>
class PeriodicToggle : public Tickable {
private:
	bool value_;
	uint32_t msecStart_;
	int nPeriod_;
	int iPeriod_;
	std::unique_ptr<uint32_t[]> msecPeriodTbl_;
public:
	PeriodicToggle() : Tickable(0, Priority::Lowest),
				value_{false}, msecStart_{0}, nPeriod_{0}, iPeriod_{0} {}
public:
	void Put(bool value);
	void StartFlip(const uint32_t msecPeriodTbl[], int nPeriod);
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "PeriodicToggle"; }
	virtual void OnTick() override;
};

template<void (*Initialize)(), void (*PutOutput)(bool)>
void PeriodicToggle<Initialize, PutOutput>::Put(bool value)
{
	value_ = value;
	PutOutput(value_);
	nPeriod_ = 0;
	msecPeriodTbl_.reset();
}

template<void (*Initialize)(), void (*PutOutput)(bool)>
void PeriodicToggle<Initialize, PutOutput>::StartFlip(const uint32_t msecPeriodTbl[], int nPeriod)
{
	msecStart_ = Tickable::GetCurrentTimeSaved();
	nPeriod_ = nPeriod;
	iPeriod_ = 0;
	msecPeriodTbl_.reset(new uint32_t[nPeriod_]);
	::memcpy(msecPeriodTbl_.get(), msecPeriodTbl, nPeriod_ * sizeof(uint32_t));
}

template<void (*Initialize)(), void (*PutOutput)(bool)>
void PeriodicToggle<Initialize, PutOutput>::OnTick()
{
	if (IsFirstTick()) Initialize();
	if (!msecPeriodTbl_) return;
	uint32_t msecCur = Tickable::GetCurrentTimeSaved();
	if (msecCur - msecStart_ < msecPeriodTbl_[iPeriod_]) return;
	iPeriod_ = (iPeriod_ + 1) % nPeriod_;
	value_ = !value_;
	PutOutput(value_);
	msecStart_ = msecCur;
}

}

#endif
