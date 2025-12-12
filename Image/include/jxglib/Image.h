//==============================================================================
// jxglib/Image.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGE_H
#define PICO_JXGLIB_IMAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Memory.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Image
//------------------------------------------------------------------------------
class Image {
public:
	struct Format {
		int bytesPerPixel;
	public:
		static const Format None;
		static const Format Bitmap;
		static const Format Gray;
		static const Format RGB;
		static const Format RGBA;
		static const Format RGB565;
		static const Format RGB565BE;
		static const Format YUV422;
	public:
		bool IsIdentical(const Format& format) const { return this == &format; }
		bool IsNone() const { return IsIdentical(None); }
		bool IsBitmap() const { return IsIdentical(Bitmap); }
		bool IsGray() const { return IsIdentical(Gray); }
		bool IsRGB() const { return IsIdentical(RGB); }
		bool IsRGBA() const { return IsIdentical(RGBA); }
		bool IsRGB565() const { return IsIdentical(RGB565); }
		bool IsRGB565BE() const { return IsIdentical(RGB565BE); }
		bool IsYUV422() const { return IsIdentical(YUV422); }
	};
public:
	template<typename T_ColorVar, typename T_ColorMem> class Getter_T {
	public:
		T_ColorVar Get(const uint8_t* p) { return T_ColorVar::zero; }
	};
	template<typename T_ColorMem, typename T_ColorVar> class Setter_T {
	public:
		void Set(uint8_t* p, const T_ColorVar& color) {}
	};
public:
	class SequencerDir {
	private:
		uint8_t value_;
	public:
		SequencerDir(uint8_t value = 0) : value_{value} {}
		SequencerDir(const SequencerDir& dir) : value_{dir.value_} {}
	public:
		// bit2: Vertical direction (0: Top to bottom, 1: Bottom to top)
		// bit1: Horizontal direction (0: Left to right, 1: Right to left)
		// bit0: Sequence dominance (0: Horizontal, 1: Vertical)
		static const uint8_t HorzFromNW	= (0 << 2) | (0 << 1) | (0 << 0);	// Rotate0
		static const uint8_t VertFromNW	= (0 << 2) | (0 << 1) | (1 << 0);	// 
		static const uint8_t HorzFromNE	= (0 << 2) | (1 << 1) | (0 << 0);	// MirrorHorz
		static const uint8_t VertFromNE	= (0 << 2) | (1 << 1) | (1 << 0);	// Rotate90/270
		static const uint8_t HorzFromSW	= (1 << 2) | (0 << 1) | (0 << 0);	// MirrorVert
		static const uint8_t VertFromSW	= (1 << 2) | (0 << 1) | (1 << 0);	// Rotate270/90
		static const uint8_t HorzFromSE	= (1 << 2) | (1 << 1) | (0 << 0);	// Rotate180
		static const uint8_t VertFromSE	= (1 << 2) | (1 << 1) | (1 << 0);	// 
		static const uint8_t Normal		= HorzFromNW;
		static const uint8_t Rotate0	= HorzFromNW;
		static const uint8_t Rotate180	= HorzFromSE;
		static const uint8_t MirrorHorz	= HorzFromNE;
		static const uint8_t MirrorVert	= HorzFromSW;
	public:
		uint8_t GetValue() const { return value_; }
	public:
		bool IsHorzFromNW() const		{ return value_ == HorzFromNW; }
		bool IsHorzFromSW() const		{ return value_ == HorzFromSW; }
		bool IsHorzFromNE() const		{ return value_ == HorzFromNE; }
		bool IsHorzFromSE() const		{ return value_ == HorzFromSE; }
		bool IsVertFromNW() const		{ return value_ == VertFromNW; }
		bool IsVertFromSW() const		{ return value_ == VertFromSW; }
		bool IsVertFromNE() const		{ return value_ == VertFromNE; }
		bool IsVertFromSE() const		{ return value_ == VertFromSE; }
		bool IsNormal() const			{ return IsHorzFromNW(); }
		bool IsRotate0() const			{ return IsHorzFromNW(); }
		bool IsRotate180() const		{ return IsHorzFromSE(); }
		bool IsMirrorHorz() const		{ return IsHorzFromNE(); }
		bool IsMirrorVert() const		{ return IsHorzFromSW(); }
		bool IsHorz() const				{ return !(value_ & (1 << 0)); }
		bool IsVert() const				{ return !!(value_ & (1 << 0)); }
		bool IsLeftToRight() const		{ return !(value_ & (1 << 1)); }
		bool IsRightToLeft() const		{ return !!(value_ & (1 << 1)); }
		bool IsTopToBottom() const		{ return !(value_ & (1 << 2)); }
		bool IsBottomToTop() const		{ return !!(value_ & (1 << 2)); }
		SequencerDir InvertHorz() const	{ return SequencerDir(value_ ^ (1 << 1)); }
		SequencerDir InvertVert() const	{ return SequencerDir(value_ ^ (1 << 2)); }
	public:
		static uint8_t SwapHV(uint8_t dir) { return ((dir & 0b100) >> 1) | ((dir & 0b010) << 1) | (dir & 0b001); } 
		SequencerDir Transform(const SequencerDir& dirTrans) const;
	};
	class ReaderDir : public SequencerDir {
	public:
		ReaderDir(uint8_t value = 0) : SequencerDir{value} {}
		ReaderDir(const SequencerDir& dir) : SequencerDir{dir} {}
	public:
		static const uint8_t Rotate90	= VertFromSW;
		static const uint8_t Rotate270	= VertFromNE;
	public:
		bool IsRotate90() const		{ return IsVertFromSW(); }
		bool IsRotate270() const	{ return IsVertFromNE(); }
	};
	class WriterDir : public SequencerDir {
	public:
		WriterDir(uint8_t value = 0) : SequencerDir{value} {}
		WriterDir(const SequencerDir& dir) : SequencerDir{dir} {}
	public:
		static const uint8_t Rotate90	= VertFromNE;
		static const uint8_t Rotate270	= VertFromSW;
	public:
		bool IsRotate90() const		{ return IsVertFromNE(); }
		bool IsRotate270() const	{ return IsVertFromSW(); }
	};
	class Sequencer {
	protected:
		uint8_t* p_;
		uint8_t* pRow_;
		int nCols_, nRows_;
		int advancePerCol_, advancePerRow_;
		int iCol_, iRow_;
	public:
		Sequencer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow);
		Sequencer(const Sequencer& sequencer);
	public:
		void MoveForward();
		void MoveForward(int nColsForward);
		bool HasDone() const { return iRow_ >= nRows_; }
	};
	class SequencerZigzag {
	protected:
		uint8_t* p_;
		int nCols_, nRows_;
		int advancePerCol_, advancePerRow_;
		int iCol_, iRow_;
	public:
		SequencerZigzag(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow);
		SequencerZigzag(const SequencerZigzag& sequencer);
	public:
		void MoveForward();
		void MoveForward(int nColsForward);
		bool HasDone() const { return iRow_ >= nRows_; }
	};
	template<typename T_Getter> class Reader : public Sequencer {
	public:
		static Reader HorzFromNW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Reader HorzFromNE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Reader HorzFromSW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Reader HorzFromSE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Reader VertFromNW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Reader VertFromNE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Reader VertFromSW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Reader VertFromSE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Reader Normal(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return HorzFromNW(image, colOffset, rowOffset, nCols, nRows);
		}
		static Reader Create(const Image& image, int colOffset, int rowOffset, int nCols, int nRows, SequencerDir dir) {
			if (dir.IsHorzFromNW()) return HorzFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromNE()) return HorzFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSW()) return HorzFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSE()) return HorzFromSE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNW()) return VertFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNE()) return VertFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSW()) return VertFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSE()) return VertFromSE(image, colOffset, rowOffset, nCols, nRows); else
			return Reader(nullptr, 0, 0, 0, 0);
		}
	public:
		Reader(const void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				Sequencer(const_cast<void*>(p), nCols, nRows, advancePerCol, advancePerRow) {}
		Reader(const Reader& reader) : Sequencer(reader) {}
		typename T_Getter::T_ColorVar ReadForward() {
			auto rtn  = T_Getter().Get(p_);
			MoveForward();
			return rtn;
		}
	};
	template<typename T_Getter> class ReaderZigzag : public SequencerZigzag {
	public:
		static ReaderZigzag HorzFromNW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static ReaderZigzag HorzFromNE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static ReaderZigzag HorzFromSW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static ReaderZigzag HorzFromSE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static ReaderZigzag VertFromNW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static ReaderZigzag VertFromNE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static ReaderZigzag VertFromSW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static ReaderZigzag VertFromSE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return ReaderZigzag(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static ReaderZigzag Normal(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return HorzFromNW(image, colOffset, rowOffset, nCols, nRows);
		}
		static ReaderZigzag Create(const Image& image, int colOffset, int rowOffset, int nCols, int nRows, SequencerDir dir) {
			if (dir.IsHorzFromNW()) return HorzFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromNE()) return HorzFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSW()) return HorzFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSE()) return HorzFromSE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNW()) return VertFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNE()) return VertFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSW()) return VertFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSE()) return VertFromSE(image, colOffset, rowOffset, nCols, nRows); else
			return ReaderZigzag(nullptr, 0, 0, 0, 0);
		}
	public:
		ReaderZigzag(const void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				SequencerZigzag(const_cast<void*>(p), nCols, nRows, advancePerCol, advancePerRow) {}
		ReaderZigzag(const ReaderZigzag& reader) : SequencerZigzag(reader) {}
		typename T_Getter::T_ColorVar ReadForward() {
			auto rtn  = T_Getter().Get(p_);
			MoveForward();
			return rtn;
		}
	};
	template<typename T_Setter> class Writer : public Sequencer {
	public:
		static Writer HorzFromNW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Writer HorzFromNE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Writer HorzFromSW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Writer HorzFromSE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Writer VertFromNW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Writer VertFromNE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Writer VertFromSW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Writer VertFromSE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Writer Normal(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return HorzFromNW(image, colOffset, rowOffset, nCols, nRows);
		}
		static Writer Create(Image& image, int colOffset, int rowOffset, int nCols, int nRows, SequencerDir dir) {
			if (dir.IsHorzFromNW()) return HorzFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromNE()) return HorzFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSW()) return HorzFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSE()) return HorzFromSE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNW()) return VertFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNE()) return VertFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSW()) return VertFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSE()) return VertFromSE(image, colOffset, rowOffset, nCols, nRows); else
			return Writer(nullptr, 0, 0, 0, 0);
		}
	public:
		Writer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				Sequencer(p, nCols, nRows, advancePerCol, advancePerRow) {}
		Writer(const Writer& writer) : Sequencer(writer) {}
	public:
		void WriteForward(const typename T_Setter::T_ColorVar& data) {
			T_Setter().Set(p_, data);
			MoveForward();
		}
	};
	template<typename T_Setter> class WriterZigzag : public SequencerZigzag {
	public:
		static WriterZigzag HorzFromNW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static WriterZigzag HorzFromNE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static WriterZigzag HorzFromSW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static WriterZigzag HorzFromSE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static WriterZigzag VertFromNW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerNW(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static WriterZigzag VertFromNE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerNE(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static WriterZigzag VertFromSW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerSW(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static WriterZigzag VertFromSE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return WriterZigzag(image.GetPointerSE(colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static WriterZigzag Normal(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return HorzFromNW(image, colOffset, rowOffset, nCols, nRows);
		}
		static WriterZigzag Create(Image& image, int colOffset, int rowOffset, int nCols, int nRows, SequencerDir dir) {
			if (dir.IsHorzFromNW()) return HorzFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromNE()) return HorzFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSW()) return HorzFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsHorzFromSE()) return HorzFromSE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNW()) return VertFromNW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromNE()) return VertFromNE(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSW()) return VertFromSW(image, colOffset, rowOffset, nCols, nRows); else
			if (dir.IsVertFromSE()) return VertFromSE(image, colOffset, rowOffset, nCols, nRows); else
			return WriterZigzag(nullptr, 0, 0, 0, 0);
		}
	public:
		WriterZigzag(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				SequencerZigzag(p, nCols, nRows, advancePerCol, advancePerRow) {}
		WriterZigzag(const WriterZigzag& writer) : SequencerZigzag(writer) {}
	public:
		void WriteForward(const typename T_Setter::T_ColorVar& data) {
			T_Setter().Set(p_, data);
			MoveForward();
		}
	};
private:
	const Format* pFormat_;
	Size size_;
	std::unique_ptr<Memory> pMemory_;
public:
	static const Image None;
public:
	Image(const Format& format = Format::None) : pFormat_{&format}, size_{0, 0} {}
	Image(const Format& format, int width, int height, const void* data = nullptr) :
			pFormat_{&format}, size_{width, height},
			pMemory_{new MemoryConst(data)} {}
	~Image();
public:
	bool IsValid() const { return !pFormat_->IsNone(); }
	void SetMemory(const Format& format, int width, int height, Memory* pMemory);
	bool Allocate(const Format& format, int width, int height);
	bool Allocate(const Format& format, const Size& size) { return Allocate(format, size.width, size.height); }
	void Free();
	void FillZero();
	const Format& GetFormat() const { return *pFormat_; }
	const Size& GetSize() const { return size_; }
	int GetWidth() const { return size_.width; }
	int GetHeight() const { return size_.height; }
	int GetBytesPerPixel() const { return pFormat_->bytesPerPixel; }
	int GetBytesPerLine() const { return GetWidth() * GetBytesPerPixel(); }
	int GetBytesBuff() const { return GetBytesPerLine() * GetHeight(); }
	uint8_t* GetPointer() { return pMemory_? pMemory_->GetPointer<uint8_t>() : nullptr; }
	const uint8_t* GetPointer() const { return pMemory_? pMemory_->GetPointer<uint8_t>() : nullptr; }
	uint8_t* GetPointer(int x, int y) { return GetPointer() + GetBytesPerPixel() * x + GetBytesPerLine() * y; }
	const uint8_t* GetPointer(int x, int y) const { return GetPointer() + GetBytesPerPixel() * x + GetBytesPerLine() * y; }
	uint8_t* GetPointerNW(int xOffset, int yOffset) { return GetPointer(xOffset, yOffset); }
	uint8_t* GetPointerNE(int xOffset, int yOffset) { return GetPointer(GetWidth() - 1 - xOffset, yOffset); }
	uint8_t* GetPointerSW(int xOffset, int yOffset) { return GetPointer(xOffset, GetHeight() - 1 - yOffset); }
	uint8_t* GetPointerSE(int xOffset, int yOffset) { return GetPointer(GetWidth() - 1 - xOffset, GetHeight() - 1 - yOffset); }
	const uint8_t* GetPointerNW(int xOffset, int yOffset) const { return GetPointer(xOffset, yOffset); }
	const uint8_t* GetPointerNE(int xOffset, int yOffset) const { return GetPointer(GetWidth() - 1 - xOffset, yOffset); }
	const uint8_t* GetPointerSW(int xOffset, int yOffset) const { return GetPointer(xOffset, GetHeight() - 1 - yOffset); }
	const uint8_t* GetPointerSE(int xOffset, int yOffset) const { return GetPointer(GetWidth() - 1 - xOffset, GetHeight() - 1 - yOffset); }
	bool IsWritable() const { return pMemory_? pMemory_->IsWritable() : false; }
public:
	static void ReadFromStream(void* context, const void* data, int size);
	static void WriteToStream(void* context, const void* data, int size);
};

template<> class Image::Getter_T<Color, Color> {
public:
	using T_ColorVar = Color;
public:
	Color Get(const uint8_t* p) { return Color(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<Color, ColorA> {
public:
	using T_ColorVar = Color;
public:
	Color Get(const uint8_t* p) { return Color(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<Color, ColorGray> {
public:
	using T_ColorVar = Color;
public:
	Color Get(const uint8_t* p) { return Color(p[0], p[0], p[0]); }
};

template<> class Image::Getter_T<Color, ColorRGB565> {
public:
	using T_ColorVar = Color;
public:
	Color Get(const uint8_t* p) { return Color(ColorRGB565(*reinterpret_cast<const uint16_t*>(p))); }
};

template<> class Image::Getter_T<ColorA, Color> {
public:
	using T_ColorVar = ColorA;
public:
	ColorA Get(const uint8_t* p) { return ColorA(p[0], p[1], p[2], 255); }
};

template<> class Image::Getter_T<ColorA, ColorA> {
public:
	using T_ColorVar = ColorA;
public:
	ColorA Get(const uint8_t* p) { return ColorA(p[0], p[1], p[2], p[3]); }
};

template<> class Image::Getter_T<ColorA, ColorGray> {
public:
	using T_ColorVar = ColorA;
public:
	ColorA Get(const uint8_t* p) { return ColorA(p[0], p[1], p[2], 255); }
};

template<> class Image::Getter_T<ColorA, ColorRGB565> {
public:
	using T_ColorVar = ColorA;
public:
	ColorA Get(const uint8_t* p) { return ColorA(ColorRGB565(*reinterpret_cast<const uint16_t*>(p))); }
};

template<> class Image::Getter_T<ColorRGB565, Color> {
public:
	using T_ColorVar = ColorRGB565;
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<ColorRGB565, ColorA> {
public:
	using T_ColorVar = ColorRGB565;
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<ColorRGB565, ColorRGB565> {
public:
	using T_ColorVar = ColorRGB565;
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(*reinterpret_cast<const uint16_t*>(p)); }
};

template<> class Image::Getter_T<ColorRGB565, ColorGray> {
public:
	using T_ColorVar = ColorRGB565;
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(p[0], p[0], p[0]); }
};

template<> class Image::Setter_T<Color, Color> {
public:
	using T_ColorVar = Color;
public:
	void Set(uint8_t* p, const Color& color) {
		p[0] = color.r, p[1] = color.g, p[2] = color.b;
	}
};

template<> class Image::Setter_T<Color, ColorA> {
public:
	using T_ColorVar = ColorA;
public:
	void Set(uint8_t* p, const Color& colorA) {
		p[0] = colorA.r, p[1] = colorA.g, p[2] = colorA.b;
	}
};

template<> class Image::Setter_T<Color, ColorRGB565> {
public:
	using T_ColorVar = ColorRGB565;
public:
	void Set(uint8_t* p, const ColorRGB565& colorRGB565) {
		Color color(colorRGB565);
		p[0] = color.r, p[1] = color.g, p[2] = color.b;
	}
};

template<> class Image::Setter_T<Color, ColorGray> {
public:
	using T_ColorVar = ColorGray;
public:
	void Set(uint8_t* p, const ColorGray& colorGray) {
		p[0] = colorGray.value, p[1] = colorGray.value, p[2] = colorGray.value;
	}
};

template<> class Image::Setter_T<ColorRGB565, Color> {
public:
	using T_ColorVar = Color;
public:
	void Set(uint8_t* p, const Color& color) {
		*reinterpret_cast<uint16_t*>(p) = ColorRGB565(color).value;
	}
};

template<> class Image::Setter_T<ColorRGB565, ColorA> {
public:
	using T_ColorVar = ColorA;
public:
	void Set(uint8_t* p, const ColorA& colorA) {
		*reinterpret_cast<uint16_t*>(p) = ColorRGB565(colorA).value;
	}
};

template<> class Image::Setter_T<ColorRGB565, ColorRGB565> {
public:
	using T_ColorVar = ColorRGB565;
public:
	void Set(uint8_t* p, const ColorRGB565& colorRGB565) {
		*reinterpret_cast<uint16_t*>(p) = colorRGB565.value;
	}
};

template<> class Image::Setter_T<ColorRGB565, ColorGray> {
public:
	using T_ColorVar = ColorGray;
public:
	void Set(uint8_t* p, const ColorGray& colorGray) {
		*reinterpret_cast<uint16_t*>(p) = ColorRGB565(colorGray.value, colorGray.value, colorGray.value).value;
	}
};

}

#endif
