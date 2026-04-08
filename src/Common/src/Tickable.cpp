//==============================================================================
// Tickable.cpp
//==============================================================================
#include <stdio.h>
#include "jxglib/Util.h"
#include "jxglib/Printable.h"
#include "jxglib/Tickable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tickable
//------------------------------------------------------------------------------
Tickable* Tickable::pTickableTop_ = nullptr;
bool Tickable::firstFlag_ = true;
bool Tickable::signalledFlag_ = false;
int Tickable::tickCalledDepth_ = 0;
int Tickable::tickCalledDepthMax_ = 0;
uint32_t Tickable::msecCurSaved_ = 0;

Tickable::Tickable(uint32_t msecTick, Priority priority) :
	msecTick_{msecTick}, priority_{priority}, msecStart_{0}, runningFlag_{true}, tickCalledFlag_{false}, pTickableNext_{nullptr}
{
	Tickable* pTickablePrev = nullptr;
	for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		if (priority > pTickable->GetPriority()) {
			break;
		} else if (priority == pTickable->GetPriority() && msecTick <= pTickable->GetTick()) {
			break;
		} 
		pTickablePrev = pTickable;
	}
	if (pTickablePrev) {
		SetNext(pTickablePrev->GetNext());
		pTickablePrev->SetNext(this);
	} else {
		SetNext(pTickableTop_);
		pTickableTop_ = this;
	}

}

bool Tickable::IsExpired(uint32_t msecCur)
{
	if (!runningFlag_) {
		msecStart_ = msecCur;
	} else if (msecCur - msecStart_ >= msecTick_) {
		msecStart_ += msecTick_;
		if (msecCur - msecStart_ >= msecTick_) {
			// If the updated msecStart_ doesn't catch up with msecCur, reset msecStart_ to msecCur.
			msecStart_ = msecCur;
		}
		return true;
	}
	return false;
}

void Tickable::ResetTick(uint32_t msecTick)
{
	msecStart_ = GetCurrentTime_();
	msecTick_ = msecTick;
}

void Tickable::RemoveFromTickable()
{
	if (pTickableTop_ == this) {
		pTickableTop_ = GetNext();
	} else {
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			if (pTickable->GetNext() == this) {
				pTickable->SetNext(GetNext());
				break;
			}
		}
	}
	SetNext(nullptr);
}

uint32_t Tickable::Tick_()
{
	bool expiredFlag = false;
	tickCalledDepth_++;
	tickCalledDepthMax_ = ChooseMax(tickCalledDepthMax_, tickCalledDepth_);
	msecCurSaved_ = GetCurrentTime_();
	if (firstFlag_) {
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			pTickable->InitTick(msecCurSaved_);
			if (!pTickable->IsTickCalled()) {
				pTickable->SetTickCalled();
				pTickable->OnTick();
				pTickable->ClearTickCalled();
			}
		}
		firstFlag_ = false;
	} else {
		for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
			if (pTickable->IsExpired(msecCurSaved_) && !pTickable->IsTickCalled()) {
				pTickable->SetTickCalled();
				pTickable->OnTick();
				pTickable->ClearTickCalled();
			}
		}
	}
	tickCalledDepth_--;
	return msecCurSaved_;
}

bool Tickable::Sleep(uint32_t msecTick)
{
	uint32_t msecStart = GetCurrentTime_();
	while (Tick_() - msecStart < msecTick) if (IsSignalled()) return true;
	return false;
}

void Tickable::PrintList(Printable& printable)
{
	int lenMax = 1;
	for (const Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		int len = ::strlen(pTickable->GetTickableName());
		if (lenMax < len) lenMax = len;
	}
	for (const Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
		printable.Printf("%-*s %-4s %dmsec\n", lenMax, pTickable->GetTickableName(), pTickable->GetPriorityName(), pTickable->GetTick());
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
	return "unk";
}

const char* Tickable::GetPriorityNameShort(Priority priority)
{
	switch (priority) {
	case Priority::Lowest:		return "Low";
	case Priority::BelowNormal:	return "BNml";
	case Priority::Normal:		return "Nml";
	case Priority::AboveNormal:	return "ANml";
	case Priority::Highest:		return "High";
	default: break;
	}
	return "unk";
}

}
