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
}

void Image::SetMemory(const Format& format, int width, int height, Memory* pMemory)
{
	pFormat_ = &format;
	width_ = width, height_ = height;
	pMemory_.reset(pMemory);
}

bool Image::Allocate(const Format& format, int width, int height)
{
	pMemory_.reset();
	void* data = ::malloc(GetBytesBuff());
	if (data) {
		SetMemory(format, width, height, new MemoryHeap(data));
		return true;
	} else {
		Free();
		return false;
	}
}

void Image::Free()
{
	pFormat_ = &Format::None;
	width_ = height_ = 0;
	pMemory_.reset();
}

void Image::FillZero()
{
	if (IsWritable()) ::memset(GetPointer(), 0x00, GetBytesBuff());
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
// Image::Sequencer
//------------------------------------------------------------------------------
Image::Sequencer::Sequencer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
	p_{reinterpret_cast<uint8_t*>(p)}, pRow_{p_}, nCols_{nCols}, nRows_{nRows},
	advancePerCol_{advancePerCol}, advancePerRow_{advancePerRow}, iCol_{0}, iRow_{0} {}

Image::Sequencer::Sequencer(const Sequencer& sequencer) :
	p_{sequencer.p_}, pRow_{sequencer.pRow_}, nCols_{sequencer.nCols_}, nRows_{sequencer.nRows_},
	advancePerCol_{sequencer.advancePerCol_}, advancePerRow_{sequencer.advancePerRow_}, iCol_{sequencer.iCol_}, iRow_{sequencer.iRow_} {}

void Image::Sequencer::MoveForward()
{
	iCol_++;
	if (iCol_ < nCols_) {
		p_ += advancePerCol_;
	} else {
		iCol_ = 0, iRow_++;
		pRow_ += advancePerRow_, p_ = pRow_;
	}
}

void Image::Sequencer::MoveForward(int nColsForward)
{
	iCol_ += nColsForward;
	if (iCol_ < nCols_) {
		p_ += advancePerCol_ * nColsForward;
	} else {
		iCol_ = 0, iRow_++;
		pRow_ += advancePerRow_, p_ = pRow_;
	}
}

//------------------------------------------------------------------------------
// Image::SequencerZigzag
//------------------------------------------------------------------------------
Image::SequencerZigzag::SequencerZigzag(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
	p_{reinterpret_cast<uint8_t*>(p)}, nCols_{nCols}, nRows_{nRows},
	advancePerCol_{advancePerCol}, advancePerRow_{advancePerRow}, iCol_{0}, iRow_{0} {}

Image::SequencerZigzag::SequencerZigzag(const SequencerZigzag& sequencer) :
	p_{sequencer.p_}, nCols_{sequencer.nCols_}, nRows_{sequencer.nRows_},
	advancePerCol_{sequencer.advancePerCol_}, advancePerRow_{sequencer.advancePerRow_}, iCol_{sequencer.iCol_}, iRow_{sequencer.iRow_} {}

void Image::SequencerZigzag::MoveForward()
{
	iCol_++;
	if (iCol_ < nCols_) {
		p_ += advancePerCol_;
	} else {
		iCol_ = 0, iRow_++;
		p_ += advancePerRow_;
		advancePerCol_ = -advancePerCol_;  // Reverse direction
	}
}

void Image::SequencerZigzag::MoveForward(int nColsForward)
{
	iCol_ += nColsForward;
	if (iCol_ < nCols_) {
		p_ += advancePerCol_ * nColsForward;
	} else {
		iCol_ = 0, iRow_++;
		p_ += advancePerRow_;
		advancePerCol_ = -advancePerCol_;  // Reverse direction
	}
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
