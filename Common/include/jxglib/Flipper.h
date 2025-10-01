//==============================================================================
// jxglib/Flipper.h
//==============================================================================
#ifndef PICO_JXGLIB_FLIPPER_H
#define PICO_JXGLIB_FLIPPER_H
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Tickable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Flipper
//------------------------------------------------------------------------------
template<void (*Initialize)(), void (*PutOutput)(bool)>
class Flipper : public Tickable {
private:
	bool value_;
	uint32_t msecStart_;
	int nPeriod_;
	int iPeriod_;
	std::unique_ptr<uint32_t[]> msecPeriodTbl_;
public:
	Flipper() : Tickable(0, Priority::Lowest),
				value_{false}, msecStart_{0}, nPeriod_{0}, iPeriod_{0} {}
public:
	void Put(bool value);
	void StartFlip(const uint32_t msecPeriodTbl[], int nPeriod);
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "Flipper"; }
	virtual void OnTick() override;
};

template<void (*Initialize)(), void (*PutOutput)(bool)>
void Flipper<Initialize, PutOutput>::Put(bool value)
{
	value_ = value;
	PutOutput(value_);
	nPeriod_ = 0;
	msecPeriodTbl_.reset();
}

template<void (*Initialize)(), void (*PutOutput)(bool)>
void Flipper<Initialize, PutOutput>::StartFlip(const uint32_t msecPeriodTbl[], int nPeriod)
{
	msecStart_ = Tickable::GetCurrentTime();
	nPeriod_ = nPeriod;
	iPeriod_ = 0;
	msecPeriodTbl_.reset(new uint32_t[nPeriod_]);
	::memcpy(msecPeriodTbl_.get(), msecPeriodTbl, nPeriod_ * sizeof(uint32_t));
}

template<void (*Initialize)(), void (*PutOutput)(bool)>
void Flipper<Initialize, PutOutput>::OnTick()
{
	if (IsFirstTick()) Initialize();
	if (!msecPeriodTbl_ || nPeriod_ == 0) return;
	uint32_t msecPeriod = msecPeriodTbl_[iPeriod_];
	if (msecPeriod == static_cast<uint32_t>(-1)) return;
	uint32_t msecCur = Tickable::GetCurrentTime();
	if (msecCur - msecStart_ < msecPeriod) return;
	iPeriod_ = (iPeriod_ + 1) % nPeriod_;
	value_ = !value_;
	PutOutput(value_);
	msecStart_ = msecCur;
}

}

#endif
