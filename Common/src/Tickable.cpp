//==============================================================================
// Tickable.cpp
//==============================================================================
#include "jxglib/Tickable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tickable
//------------------------------------------------------------------------------
Tickable* Tickable::pTickableTop_ = nullptr;
bool Tickable::firstFlag_ = true;
uint32_t Tickable::msecMainStart_ = 0;

Tickable::Tickable(uint32_t msecTick) : msecTick_{msecTick}, msecStart_{0}, pTickableNext_{pTickableTop_}
{
	pTickableTop_ = this;
}

bool Tickable::IsExpired(uint32_t msecCur)
{
	if (msecTick_ == -1 || msecCur - msecStart_ < msecTick_) return false;
	msecStart_ += msecTick_;
	return true;
}

bool Tickable::Tick(uint32_t msecTick)
{
	uint32_t msecCur = ::to_ms_since_boot(::get_absolute_time());
	if (firstFlag_) {
		firstFlag_ = false;
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			pTickable->InitTick(msecCur);
			pTickable->OnTick();
		}
		msecMainStart_ = msecCur;
		return true;
	} else {
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			if (pTickable->IsExpired(msecCur)) pTickable->OnTick();
		}
		if (msecTick == -1 || msecCur - msecMainStart_ < msecTick) return false;
		msecMainStart_ += msecTick;
		return true;
	}
}

}
