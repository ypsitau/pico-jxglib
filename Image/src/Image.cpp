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
	pFormat_ = &format;
	width_ = width, height_ = height;
	bytesPerLine_ = width * pFormat_->bytesPerPixel,
	data_ = reinterpret_cast<uint8_t*>(::malloc(GetBytesBuff()));
	allocatedFlag_ = true;
	return !data_;
}

void Image::FillZero()
{
	::memset(data_, 0x00, GetBytesBuff());
}

void Image::CopyRegion(int xDst, int yDst, int xSrc, int ySrc, int width, int height)
{
	
}

}
