//==============================================================================
// Image.cpp
//==============================================================================
#include "jxglib/Image.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Image
//------------------------------------------------------------------------------
const Image::Format Image::Format::None {0};
const Image::Format Image::Format::Bitmap {0};
const Image::Format Image::Format::Gray {1};
const Image::Format Image::Format::RGB {3};
const Image::Format Image::Format::RGBA {4};
const Image::Format Image::Format::RGB565 {2};

Image::~Image()
{
	if (allocatedFlag_) ::free(data_);
}
bool Image::Alloc(const Format& format, int width, int height)
{
	if (allocatedFlag_) ::free(data_);
	allocatedFlag_ = true;
	pFormat_ = &format;
	width_ = width, height_ = height;
	data_ = reinterpret_cast<uint8_t*>(::malloc(GetBytesBuff()));
	return !!data_;
}

void Image::Free()
{
	if (!allocatedFlag_) return;
	allocatedFlag_ = false;
	pFormat_ = &Format::None;
	width_ = height_ = 0;
	::free(data_);
	data_ = nullptr;
}

void Image::FillZero()
{
	::memset(data_, 0x00, GetBytesBuff());
}

}
