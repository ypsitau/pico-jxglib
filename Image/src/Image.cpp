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
const Image::Format Image::Format::RGB565BE {2};

Image::~Image()
{
	if (allocatedFlag_) ::free(data_);
}
bool Image::Allocate(const Format& format, int width, int height)
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

void Image::ReadFromStream(void* context, const void* data, int size)
{
}

void Image::WriteToStream(void* context, const void* data, int size)
{
	Stream* pStream = reinterpret_cast<Stream*>(context);
	pStream->Write(data, size);
}

//------------------------------------------------------------------------------
// Image::SequencerDir
//------------------------------------------------------------------------------
Image::SequencerDir Image::SequencerDir::Transform(const SequencerDir& dirTrans) const
{
	switch (dirTrans.GetValue()) {
	case HorzFromNW: return SequencerDir(value_);
	case VertFromNW: return SequencerDir(value_ ^ 0b001);
	case HorzFromNE: return SequencerDir(SwapHV(value_) ^ 0b010);
	case VertFromNE: return SequencerDir(SwapHV(value_) ^ 0b011);
	case HorzFromSW: return SequencerDir(SwapHV(value_) ^ 0b100);
	case VertFromSW: return SequencerDir(SwapHV(value_) ^ 0b101);
	case HorzFromSE: return SequencerDir(value_ ^ 0b110);
	case VertFromSE: return SequencerDir(value_ ^ 0b111);
	default: return SequencerDir(value_);
	}
}

}
