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

const Image::SequencerDir Image::SequencerDir::HorzFromNW	{(0 << 2) | (0 << 1) | (0 << 0)};
const Image::SequencerDir Image::SequencerDir::HorzFromSW	{(1 << 2) | (0 << 1) | (0 << 0)};
const Image::SequencerDir Image::SequencerDir::HorzFromNE	{(0 << 2) | (1 << 1) | (0 << 0)};
const Image::SequencerDir Image::SequencerDir::HorzFromSE	{(1 << 2) | (1 << 1) | (0 << 0)};
const Image::SequencerDir Image::SequencerDir::VertFromNW	{(0 << 2) | (0 << 1) | (1 << 0)};
const Image::SequencerDir Image::SequencerDir::VertFromSW	{(1 << 2) | (0 << 1) | (1 << 0)};
const Image::SequencerDir Image::SequencerDir::VertFromNE	{(0 << 2) | (1 << 1) | (1 << 0)};
const Image::SequencerDir Image::SequencerDir::VertFromSE	{(1 << 2) | (1 << 1) | (1 << 0)};
const Image::SequencerDir Image::SequencerDir::Normal		{HorzFromNW};
const Image::SequencerDir Image::SequencerDir::MirrorHorz	{HorzFromNE};
const Image::SequencerDir Image::SequencerDir::MirrorVert	{HorzFromSW};
const Image::SequencerDir Image::SequencerDir::Rotate0		{HorzFromNW};
const Image::SequencerDir Image::SequencerDir::Rotate180	{HorzFromSE};

const Image::SequencerDir Image::ReaderDir::Rotate90		{VertFromSW};
const Image::SequencerDir Image::ReaderDir::Rotate270		{VertFromNE};

const Image::SequencerDir Image::WriterDir::Rotate90		{VertFromNE};
const Image::SequencerDir Image::WriterDir::Rotate270		{VertFromSW};

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
