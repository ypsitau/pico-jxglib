//==============================================================================
// Camera.cpp
//==============================================================================
#include "jxglib/Camera.h"

namespace jxglib::Camera {

const Size Size_VGA(640, 480);
const Size Size_QVGA(320, 240);
const Size Size_QQVGA(160, 120);
const Size Size_QQQVGA(80, 60);
const Size Size_CIF(352, 288);
const Size Size_QCIF(176, 144);
const Size Size_QQCIF(88, 72);

//------------------------------------------------------------------------------
// Camera::Base
//------------------------------------------------------------------------------
Base::Base(bool registerFlag) : IntrusiveListNode<Base>(registerFlag), format_{Format::None},
		capturedFlag_{false}, usecPerFrame_{0}, timeLastCapture_{0}
{
}

bool Base::IsValid() const
{
	return this != &Dummy::Instance;
}

void Base::UpdateSize()
{
	size_ =
		(resolution_ == Resolution::VGA)?		Size_VGA :
		(resolution_ == Resolution::QVGA)?		Size_QVGA :
		(resolution_ == Resolution::QQVGA)?		Size_QQVGA :
		(resolution_ == Resolution::QQQVGA)?	Size_QQQVGA :
		(resolution_ == Resolution::CIF)?		Size_CIF :
		(resolution_ == Resolution::QCIF)?		Size_QCIF :
		(resolution_ == Resolution::QQCIF)?		Size_QQCIF : Size_QVGA;
}

Base& Base::SetResolution(Resolution resolution)
{
	resolution_ = resolution;
	UpdateSize();
	return *this;
}

const Image& Base::Capture()
{
	usecPerFrame_ = capturedFlag_? ::absolute_time_diff_us(timeLastCapture_, ::get_absolute_time()) : 0;
	capturedFlag_ = false;
	DoCapture();
	capturedFlag_ = true;
	timeLastCapture_ = ::get_absolute_time();
	return GetImage();
}

//------------------------------------------------------------------------------
// Dummy
//------------------------------------------------------------------------------
Dummy Dummy::Instance;

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
Base& N(int iCamera)
{
	Base* pBase = Base::GetListNodeHead();
	for (int i = 0; pBase; pBase = pBase->GetListNodeNext(), i++) {
		if (i == iCamera) return *pBase;
	}
	return Dummy::Instance;
}

}
