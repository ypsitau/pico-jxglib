//==============================================================================
// jxglib/Image.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGE_H
#define PICO_JXGLIB_IMAGE_H
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
	template<typename ColorVar_T, typename ColorMem_T> class Getter_T {
	public:
		using Type = ColorVar_T;
	public:
		ColorVar_T Get(const uint8_t* p) { return ColorVar_T::zero; }
	};
	template<typename ColorMem_T, typename ColorVar_T> class Setter_T {
	public:
		using Type = ColorVar_T;
	public:
		void Set(uint8_t* p, const ColorVar_T& color) {}
	};
	class GetColor_SrcRGB {
	public:
		using Type = Color;
	public:
		Type operator()(const uint8_t* p) { return Color(p[0], p[1], p[2]); }
	};
	class GetColor_SrcRGB565 {
	public:
		using Type = Color;
	public:
		Type operator()(const uint8_t* p) {  return Type(ColorRGB565(*reinterpret_cast<const uint16_t*>(p))); }
	};
	class GetColorA_SrcRGB565 {
	public:
		using Type = ColorRGB565;
	public:
		Type operator()(const uint8_t* p) { return Type(*reinterpret_cast<const uint16_t*>(p)); }
	};
	class GetColorRGB565_SrcGray {
	public:
		using Type = ColorRGB565;
	public:
		Type operator()(const uint8_t* p) { return ColorRGB565(p[0], p[0], p[0]); }
	};
	class GetColorRGB565_SrcRGB {
	public:
		using Type = ColorRGB565;
	public:
		Type operator()(const uint8_t* p) { return ColorRGB565(p[0], p[1], p[2]); }
	};
	class GetColorRGB565_SrcRGBA {
	public:
		using Type = ColorRGB565;
	public:
		Type operator()(const uint8_t* p) { return ColorRGB565(p[0], p[1], p[2]); }
	};
	class GetColorRGB565_SrcRGB565 {
	public:
		using Type = ColorRGB565;
	public:
		Type operator()(const uint8_t* p) { return ColorRGB565(*reinterpret_cast<const uint16_t*>(p)); }
	};
public:
	class PutColorRGB565_DstRGB565 {
	public:
		using Type = ColorRGB565;
	public:
		void operator()(uint8_t* p, const Type& data) { *reinterpret_cast<uint16_t*>(p) = data.value; }
	};
public:
	enum class SequencerDir {
		Horz,
		HorzFromNW	= Horz,
		HorzFromNE,
		HorzFromSW,
		HorzFromSE,
		Vert,
		VertFromNW	= Vert,
		VertFromNE,
		VertFromSW,
		VertFromSE,
		Normal		= HorzFromNW,
		Rotate0		= HorzFromNW,
		Rotate90	= VertFromSW,
		Rotate180	= HorzFromSE,
		Rotate270	= VertFromNE,
		MirrorHorz	= HorzFromNE,
		MirrorVert	= HorzFromSW,
	};
	using ReaderDir = SequencerDir;
	using WriterDir = SequencerDir;
	class Sequencer {
	protected:
		int nCols_, nRows_;
		int advancePerCol_, advancePerRow_;
		uint8_t* p_;
		uint8_t* pRow_;
		int iCol_, iRow_;
	public:
		Sequencer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
			p_{reinterpret_cast<uint8_t*>(p)}, pRow_{p_}, nCols_{nCols}, nRows_{nRows},
			advancePerCol_{advancePerCol}, advancePerRow_{advancePerRow}, iCol_{0}, iRow_{0} {}
		void MoveForward(int nColsForward = 1) {
			iCol_ += nColsForward;
			p_ += advancePerCol_ * nColsForward;
			if (iCol_ >= nCols_) {
				iCol_ = 0, iRow_++;
				pRow_ += advancePerRow_, p_ = pRow_;
			}
		}
		bool HasDone() const { return iRow_ >= nRows_; }
	};
	template<typename T_GetColor> class Reader : public Sequencer {
	public:
		static Reader HorzFromNW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointer(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Reader HorzFromNE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointer(nCols - 1 - colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Reader HorzFromSW(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointer(colOffset, nRows - 1 - rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Reader HorzFromSE(const Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Reader(image.GetPointer(nCols - 1 - colOffset, nRows - 1 - rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Reader VertFromNW(const Image& image, int rowOffset, int colOffset, int nRows, int nCols) {
			return Reader(image.GetPointer(rowOffset, colOffset),
					nCols, nRows, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Reader VertFromNE(const Image& image, int rowOffset, int colOffset, int nRows, int nCols) {
			return Reader(image.GetPointer(nRows - 1 - rowOffset, colOffset),
					nCols, nRows, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Reader VertFromSW(const Image& image, int rowOffset, int colOffset, int nRows, int nCols) {
			return Reader(image.GetPointer(rowOffset, nCols - 1 - colOffset),
					nCols, nRows, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Reader VertFromSE(const Image& image, int rowOffset, int colOffset, int nRows, int nCols) {
			return Reader(image.GetPointer(nRows - 1 - rowOffset, nCols - 1 - colOffset),
					nCols, nRows, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Reader Create(const Image& image, int xOffset, int yOffset, int width, int height, ReaderDir dir) {
			switch (dir) {
			case ReaderDir::HorzFromNW: return HorzFromNW(image, xOffset, yOffset, width, height);
			case ReaderDir::HorzFromNE: return HorzFromNE(image, xOffset, yOffset, width, height);
			case ReaderDir::HorzFromSW: return HorzFromSW(image, xOffset, yOffset, width, height);
			case ReaderDir::HorzFromSE: return HorzFromSE(image, xOffset, yOffset, width, height);
			case ReaderDir::VertFromNW: return VertFromNW(image, xOffset, yOffset, width, height);
			case ReaderDir::VertFromNE: return VertFromNE(image, xOffset, yOffset, width, height);
			case ReaderDir::VertFromSW: return VertFromSW(image, xOffset, yOffset, width, height);
			case ReaderDir::VertFromSE: return VertFromSE(image, xOffset, yOffset, width, height);
			default: break;
			}
			return Reader(nullptr, 0, 0, 0, 0);
		}
	public:
		Reader(const void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				Sequencer(const_cast<void*>(p), nCols, nRows, advancePerCol, advancePerRow) {}
		typename T_GetColor::Type ReadForward() {
			auto rtn  = T_GetColor()(p_);
			MoveForward();
			return rtn;
		}
	};
	template<typename T_PutColor> class Writer : public Sequencer {
	public:
		static Writer HorzFromNW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(colOffset, rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Writer HorzFromNE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(nCols - 1 - colOffset, rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Writer HorzFromSW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(colOffset, nRows - 1 - rowOffset),
					nCols, nRows, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Writer HorzFromSE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(nCols - 1 - colOffset, nRows - 1 - rowOffset),
					nCols, nRows, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Writer VertFromNW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(rowOffset, colOffset),
					nCols, nRows, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Writer VertFromNE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(rowOffset, nCols - 1 - colOffset),
					nCols, nRows, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Writer VertFromSW(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(nRows - 1 - rowOffset, nCols),
					nCols, nRows, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Writer VertFromSE(Image& image, int colOffset, int rowOffset, int nCols, int nRows) {
			return Writer(image.GetPointer(nRows - 1 - rowOffset, nCols - 1 - colOffset),
					nCols, nRows, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Writer Create(Image& image, int colOffset, int rowOffset, int nCols, int nRows, WriterDir dir) {
			switch (dir) {
			case WriterDir::HorzFromNW: return HorzFromNW(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::HorzFromNE: return HorzFromNE(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::HorzFromSW: return HorzFromSW(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::HorzFromSE: return HorzFromSE(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::VertFromNW: return VertFromNW(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::VertFromNE: return VertFromNE(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::VertFromSW: return VertFromSW(image, colOffset, rowOffset, nCols, nRows);
			case WriterDir::VertFromSE: return VertFromSE(image, colOffset, rowOffset, nCols, nRows);
			default: break;
			}
			return Writer(nullptr, 0, 0, 0, 0);
		}
	public:
		Writer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				Sequencer(p, nCols, nRows, advancePerCol, advancePerRow) {}
		void WriteForward(const typename T_PutColor::Type& data) {
			T_PutColor()(p_, data);
			MoveForward();
		}
	};
private:
	const Format* pFormat_;
	int width_;
	int height_;
	int bytesPerLine_;
	uint8_t* data_;
	bool allocatedFlag_;
public:
	Image() : pFormat_{&Format::None}, width_{0}, height_{0}, bytesPerLine_{0},
			data_{nullptr}, allocatedFlag_{false} {}
	Image(const Format& format, int width, int height, const void* data = nullptr) :
			pFormat_{&format}, width_{width}, height_{height},
			bytesPerLine_{width * pFormat_->bytesPerPixel},
			data_{reinterpret_cast<uint8_t*>(const_cast<void*>(data))}, allocatedFlag_{false} {}
	~Image();
	bool Alloc(const Format& format, int width, int height);
	void FillZero();
	const Format& GetFormat() const { return *pFormat_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	int GetBytesPerPixel() const { return pFormat_->bytesPerPixel; }
	int GetBytesPerLine() const { return bytesPerLine_; }
	int GetBytesBuff() const { return GetBytesPerLine() * GetHeight(); }
	uint8_t* GetPointer() { return data_; }
	const uint8_t* GetPointer() const { return data_; }
	uint8_t* GetPointer(int x, int y) {
		return data_ + GetBytesPerPixel() * x + GetBytesPerLine() * y;
	}
	const uint8_t* GetPointer(int x, int y) const {
		return data_ + GetBytesPerPixel() * x + GetBytesPerLine() * y;
	}
	bool IsWritable() const { return allocatedFlag_; }
	static bool IsDirHorz(SequencerDir dir) { return dir < SequencerDir::Vert; }
	static bool IsDirVert(SequencerDir dir) { return dir >= SequencerDir::Vert; }
};

template<> class Image::Getter_T<Color, Color> {
public:
	Color Get(const uint8_t* p) { return Color(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<Color, ColorA> {
public:
	Color Get(const uint8_t* p) { return Color(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<Color, ColorGray> {
public:
	Color Get(const uint8_t* p) { return Color(p[0], p[0], p[0]); }
};

template<> class Image::Getter_T<Color, ColorRGB565> {
public:
	Color Get(const uint8_t* p) { return Color(ColorRGB565(*reinterpret_cast<const uint16_t*>(p))); }
};

template<> class Image::Getter_T<ColorA, Color> {
public:
	ColorA Get(const uint8_t* p) { return ColorA(p[0], p[1], p[2], 255); }
};

template<> class Image::Getter_T<ColorA, ColorA> {
public:
	ColorA Get(const uint8_t* p) { return ColorA(p[0], p[1], p[2], p[3]); }
};

template<> class Image::Getter_T<ColorA, ColorGray> {
public:
	ColorA Get(const uint8_t* p) { return ColorA(p[0], p[1], p[2], 255); }
};

template<> class Image::Getter_T<ColorA, ColorRGB565> {
public:
	ColorA Get(const uint8_t* p) { return ColorA(ColorRGB565(*reinterpret_cast<const uint16_t*>(p))); }
};

template<> class Image::Getter_T<ColorRGB565, Color> {
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<ColorRGB565, ColorA> {
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(p[0], p[1], p[2]); }
};

template<> class Image::Getter_T<ColorRGB565, ColorRGB565> {
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(*reinterpret_cast<const uint16_t*>(p)); }
};

template<> class Image::Getter_T<ColorRGB565, ColorGray> {
public:
	ColorRGB565 Get(const uint8_t* p) { return ColorRGB565(p[0], p[0], p[0]); }
};

template<> class Image::Setter_T<Color, Color> {
public:
	void Set(uint8_t* p, const Color& color) {
		p[0] = color.r, p[1] = color.g, p[2] = color.b;
	}
};

template<> class Image::Setter_T<Color, ColorA> {
public:
	void Set(uint8_t* p, const Color& colorA) {
		p[0] = colorA.r, p[1] = colorA.g, p[2] = colorA.b;
	}
};

template<> class Image::Setter_T<Color, ColorRGB565> {
public:
	void Set(uint8_t* p, const ColorRGB565& colorRGB565) {
		Color color(colorRGB565);
		p[0] = color.r, p[1] = color.g, p[2] = color.b;
	}
};

template<> class Image::Setter_T<Color, ColorGray> {
public:
	void Set(uint8_t* p, const ColorGray& colorGray) {
		p[0] = colorGray.value, p[1] = colorGray.value, p[2] = colorGray.value;
	}
};

template<> class Image::Setter_T<ColorRGB565, Color> {
public:
	void Set(uint8_t* p, const Color& color) {
		*reinterpret_cast<uint16_t*>(p) = ColorRGB565(color).value;
	}
};

template<> class Image::Setter_T<ColorRGB565, ColorA> {
public:
	void Set(uint8_t* p, const ColorA& colorA) {
		*reinterpret_cast<uint16_t*>(p) = ColorRGB565(colorA).value;
	}
};

template<> class Image::Setter_T<ColorRGB565, ColorRGB565> {
public:
	void Set(uint8_t* p, const ColorRGB565& colorRGB565) {
		*reinterpret_cast<uint16_t*>(p) = colorRGB565.value;
	}
};

template<> class Image::Setter_T<ColorRGB565, ColorGray> {
public:
	void Set(uint8_t* p, const ColorGray& colorGray) {
		*reinterpret_cast<uint16_t*>(p) = ColorRGB565(
					colorGray.value, colorGray.value, colorGray.value);
	}
};

}

#endif
