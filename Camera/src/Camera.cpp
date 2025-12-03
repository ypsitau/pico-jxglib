//==============================================================================
// Camera.cpp
//==============================================================================
#include "jxglib/Camera.h"

namespace jxglib::Camera {

//------------------------------------------------------------------------------
// Camera::Base
//------------------------------------------------------------------------------
Base* Base::pHead_ = nullptr;

Base::Base() : pNext_{nullptr}
{
	if (IsDummy()) {
		// nothing to do
	} else if (pHead_) {
		Base* pBase = pHead_;
		for ( ; pBase->pNext_; pBase = pBase->pNext_);
		pBase->pNext_ = this;
	} else {
		pHead_ = this;
	}
}

Base::~Base()
{
	if (pHead_ == this) {
		pHead_ = pNext_;
	} else {
		Base* pBase = pHead_;
		for ( ; pBase; pBase = pBase->pNext_) {
			if (pBase->pNext_ == this) {
				pBase->pNext_ = pNext_;
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Dummy
//------------------------------------------------------------------------------
Dummy Dummy::Instance;

}
