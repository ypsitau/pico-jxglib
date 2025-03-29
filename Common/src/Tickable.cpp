//==============================================================================
// Tickable.cpp
//==============================================================================
#include <stdio.h>
#include "jxglib/Printable.h"
#include "jxglib/Tickable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tickable
//------------------------------------------------------------------------------
Tickable* Tickable::pTickableTop_ = nullptr;
bool Tickable::firstFlag_ = true;
uint32_t Tickable::msecMainStart_ = 0;

Tickable::Tickable(uint32_t msecTick, Priority priority) :
	msecTick_{msecTick}, priority_{priority}, msecStart_{0}, runningFlag_{true}, pTickableNext_{pTickableTop_}
{
	Tickable* pTickablePrev = nullptr;
	for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		if (GetPriority() >= pTickable->GetPriority()) break;
		pTickablePrev = pTickable;
	}
	if (pTickablePrev) {
		SetNext(pTickablePrev->GetNext());
		pTickablePrev->SetNext(this);
	} else {
		pTickableTop_ = this;
	}

}

bool Tickable::IsExpired(uint32_t msecCur)
{
	if (!runningFlag_) {
		msecStart_ = msecCur;
	} else if (msecCur - msecStart_ >= msecTick_) {
		msecStart_ += msecTick_;
		return true;
	}
	return false;
}

void Tickable::ResetTick(uint32_t msecTick)
{
	msecStart_ = GetCurrentTime();
	msecTick_ = msecTick;
}

bool Tickable::Tick(uint32_t msecTick)
{
	uint32_t msecCur = GetCurrentTime();
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

void Tickable::PrintList(Printable& printable)
{
	int lenMax = 1;
	for (const Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		int len = ::strlen(pTickable->GetTickableName());
		if (lenMax < len) lenMax = len;
	}
	for (const Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		printable.Printf("%-*s %-12s %dmsec\n", lenMax, pTickable->GetTickableName(), pTickable->GetPriorityName(), pTickable->GetTick());
	}
}

const char* Tickable::GetPriorityName(Priority priority)
{
	switch (priority) {
	case Priority::Lowest:		return "Lowest";
	case Priority::BelowNormal:	return "BelowNormal";
	case Priority::Normal:		return "Normal";
	case Priority::AboveNormal:	return "AboveNormal";
	case Priority::Highest:		return "Highest";
	default: break;
	}
	return "unknown";
}

}
