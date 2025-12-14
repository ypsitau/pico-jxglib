//==============================================================================
// VideoTransmitter.cpp
//==============================================================================
#include "jxglib/VideoTransmitter.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VideoTransmitter
//------------------------------------------------------------------------------
VideoTransmitter* VideoTransmitter::pHead_;

VideoTransmitter::VideoTransmitter()
{
	if (this == &VideoTransmitterDummy::Instance) {
		// nothing to do
	} else if (pHead_) {
		VideoTransmitter* pVideoTransmitter = pHead_;
		for ( ; pVideoTransmitter->pNext_; pVideoTransmitter = pVideoTransmitter->pNext_);
		pVideoTransmitter->pNext_ = this;
	} else {
		pHead_ = this;
	}
}

VideoTransmitter::~VideoTransmitter()
{
	if (pHead_ == this) {
		pHead_ = pNext_;
	} else {
		VideoTransmitter* pVideoTransmitter = pHead_;
		for ( ; pVideoTransmitter; pVideoTransmitter = pVideoTransmitter->pNext_) {
			if (pVideoTransmitter->pNext_ == this) {
				pVideoTransmitter->pNext_ = pNext_;
				break;
			}
		}
	}
}

bool VideoTransmitter::IsValid() const
{
	return this != &VideoTransmitterDummy::Instance;
}

VideoTransmitter& VideoTransmitter::GetInstance(int iVideoTransmitter)
{
	VideoTransmitter* pVideoTransmitter = GetHead();
	for (int i = 0; pVideoTransmitter; pVideoTransmitter = pVideoTransmitter->GetNext(), i++) {
		if (i == iVideoTransmitter) return *pVideoTransmitter;
	}
	return VideoTransmitterDummy::Instance;
}

//------------------------------------------------------------------------------
// VideoTransmitterDummy
//------------------------------------------------------------------------------
VideoTransmitterDummy VideoTransmitterDummy::Instance;

}
