//==============================================================================
// jxglib/Image.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGE_H
#define PICO_JXGLIB_IMAGE_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Color.h"

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
	public:
		bool IsIdentical(const Format& format) const { return this == &format; }
		bool IsNone() const { return IsIdentical(None); }
		bool IsBitmap() const { return IsIdentical(Bitmap); }
		bool IsGray() const { return IsIdentical(Gray); }
		bool IsRGB() const { return IsIdentical(RGB); }
		bool IsRGBA() const { return IsIdentical(RGBA); }
		bool IsRGB565() const { return IsIdentical(RGB565); }
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
		static const SequencerDir HorzFromNW;
		static const SequencerDir HorzFromSW;
		static const SequencerDir HorzFromNE;
		static const SequencerDir HorzFromSE;
		static const SequencerDir VertFromNW;
		static const SequencerDir VertFromSW;
		static const SequencerDir VertFromNE;
		static const SequencerDir VertFromSE;
		static const SequencerDir Normal;
		static const SequencerDir Rotate0;
		static const SequencerDir Rotate180;
		static const SequencerDir MirrorHorz;
		static const SequencerDir MirrorVert;
	public:
		uint8_t GetValue() const { return value_; }
	public:
		bool IsHorzFromNW() const { return value_ == HorzFromNW.value_; }
		bool IsHorzFromSW() const { return value_ == HorzFromSW.value_; }
		bool IsHorzFromNE() const { return value_ == HorzFromNE.value_; }
		bool IsHorzFromSE() const { return value_ == HorzFromSE.value_; }
		bool IsVertFromNW() const { return value_ == VertFromNW.value_; }
		bool IsVertFromSW() const { return value_ == VertFromSW.value_; }
		bool IsVertFromNE() const { return value_ == VertFromNE.value_; }
		bool IsVertFromSE() const { return value_ == VertFromSE.value_; }
		bool IsHorz() { return !(value_ & (1 << 0)); }
		bool IsVert() { return !!(value_ & (1 << 0)); }
		bool IsLeftToRight() { return !(value_ & (1 << 1)); }
		bool IsRightToLeft() { return !!(value_ & (1 << 1)); }
		bool IsTopToBottom() { return !(value_ & (1 << 2)); }
		bool IsBottomToTop() { return !!(value_ & (1 << 2)); }
		SequencerDir InvertHorz() { return SequencerDir(value_ ^ (1 << 1)); }
		SequencerDir InvertVert() { return SequencerDir(value_ ^ (1 << 2)); }
	};
	class ReaderDir : public SequencerDir {
	public:
		ReaderDir(uint8_t value = 0) : SequencerDir{value} {}
		ReaderDir(const SequencerDir& dir) : SequencerDir{dir} {}
	public:
		static const SequencerDir Rotate90;
		static const SequencerDir Rotate270;

	};
	class WriterDir : public SequencerDir {
	public:
		WriterDir(uint8_t value = 0) : SequencerDir{value} {}
		WriterDir(const SequencerDir& dir) : SequencerDir{dir} {}
	public:
		static const SequencerDir Rotate90;
		static const SequencerDir Rotate270;

	};
	class Sequencer {
	protected:
		uint8_t* p_;
		uint8_t* pRow_;
		int nCols_, nRows_;
		int advancePerCol_, advancePerRow_;
		int iCol_, iRow_;
	public:
		Sequencer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
			p_{reinterpret_cast<uint8_t*>(p)}, pRow_{p_}, nCols_{nCols}, nRows_{nRows},
			advancePerCol_{advancePerCol}, advancePerRow_{advancePerRow}, iCol_{0}, iRow_{0} {}
		Sequencer(const Sequencer& sequencer) :
			p_{sequencer.p_}, pRow_{sequencer.pRow_}, nCols_{sequencer.nCols_}, nRows_{sequencer.nRows_},
			advancePerCol_{sequencer.advancePerCol_}, advancePerRow_{sequencer.advancePerRow_}, iCol_{sequencer.iCol_}, iRow_{sequencer.iRow_} {}

		void MoveForward() {
			iCol_++;
			p_ += advancePerCol_;
			if (iCol_ >= nCols_) {
				iCol_ = 0, iRow_++;
				pRow_ += advancePerRow_, p_ = pRow_;
			}
		}
		void MoveForward(int nColsForward) {
			iCol_ += nColsForward;
			p_ += advancePerCol_ * nColsForward;
			if (iCol_ >= nCols_) {
				iCol_ = 0, iRow_++;
				pRow_ += advancePerRow_, p_ = pRow_;
			}
		}
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
private:
	bool allocatedFlag_;
	const Format* pFormat_;
	int width_;
	int height_;
	uint8_t* data_;
public:
	Image() : allocatedFlag_{false}, pFormat_{&Format::None}, width_{0}, height_{0}, data_{nullptr} {}
	Image(const Format& format, int width, int height, const void* data = nullptr) :
			allocatedFlag_{false}, pFormat_{&format}, width_{width}, height_{height},
			data_{reinterpret_cast<uint8_t*>(const_cast<void*>(data))} {}
	~Image();
	bool Alloc(const Format& format, int width, int height);
	void Free();
	void FillZero();
	const Format& GetFormat() const { return *pFormat_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	int GetBytesPerPixel() const { return pFormat_->bytesPerPixel; }
	int GetBytesPerLine() const { return GetWidth() * GetBytesPerPixel(); }
	int GetBytesBuff() const { return GetBytesPerLine() * GetHeight(); }
	uint8_t* GetPointer() { return data_; }
	const uint8_t* GetPointer() const { return data_; }
	uint8_t* GetPointer(int x, int y) {
		return data_ + GetBytesPerPixel() * x + GetBytesPerLine() * y;
	}
	const uint8_t* GetPointer(int x, int y) const {
		return data_ + GetBytesPerPixel() * x + GetBytesPerLine() * y;
	}
	uint8_t* GetPointerNW(int xOffset, int yOffset) { return GetPointer(xOffset, yOffset); }
	uint8_t* GetPointerNE(int xOffset, int yOffset) { return GetPointer(GetWidth() - 1 - xOffset, yOffset); }
	uint8_t* GetPointerSW(int xOffset, int yOffset) { return GetPointer(xOffset, GetHeight() - 1 - yOffset); }
	uint8_t* GetPointerSE(int xOffset, int yOffset) { return GetPointer(GetWidth() - 1 - xOffset, GetHeight() - 1 - yOffset); }
	const uint8_t* GetPointerNW(int xOffset, int yOffset) const { return GetPointer(xOffset, yOffset); }
	const uint8_t* GetPointerNE(int xOffset, int yOffset) const { return GetPointer(GetWidth() - 1 - xOffset, yOffset); }
	const uint8_t* GetPointerSW(int xOffset, int yOffset) const { return GetPointer(xOffset, GetHeight() - 1 - yOffset); }
	const uint8_t* GetPointerSE(int xOffset, int yOffset) const { return GetPointer(GetWidth() - 1 - xOffset, GetHeight() - 1 - yOffset); }
	bool IsWritable() const { return allocatedFlag_; }
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
