//==============================================================================
// Tickable.cpp
//==============================================================================
#include "jxglib/Tickable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tickable
//------------------------------------------------------------------------------
bool Tickable::IsExpired(uint32_t msecCur)
{
	if (msecTick_ == -1 || msecCur - msecStart_ < msecTick_) return false;
	msecStart_ += msecTick_;
	return true;
}

//------------------------------------------------------------------------------
// Tickable::Master
//------------------------------------------------------------------------------
Tickable::Master Tickable::Master::Instance;

void Tickable::Master::AddTickable(Tickable* pTickable)
{
	if (!pTickableTop_) {
		pTickableTop_ = pTickable;
		return;
	}
	Tickable* pTickableLast = pTickableTop_;
	for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		pTickableLast = pTickable;
	}
	pTickableLast->SetNext(pTickable);
}

void Tickable::Master::Tick()
{
	uint32_t msecCur = ::to_ms_since_boot(::get_absolute_time());
	if (firstFlag_) {
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			pTickable->InitTick(msecCur);
			pTickable->OnTick();
		}
		firstFlag_ = false;
	} else {
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			if (pTickable->IsExpired(msecCur)) pTickable->OnTick();
		}
	}
}

}
