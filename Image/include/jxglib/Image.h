//==============================================================================
// jxglib/Image.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGE_H
#define PICO_JXGLIB_IMAGE_H
#include "pico/stdlib.h"
#include "jxglib/Color.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Image
//------------------------------------------------------------------------------
class Image {
public:
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
		HorzFromNW = Horz,
		HorzFromNE,
		HorzFromSW,
		HorzFromSE,
		Vert,
		VertFromNW = Vert,
		VertFromNE,
		VertFromSW,
		VertFromSE,
	};
	using ReaderDir = SequencerDir;
	using WriterDir = SequencerDir;
	struct Draw {
		static const ReaderDir Normal = ReaderDir::HorzFromNW;
		static const ReaderDir Rotate0 = ReaderDir::HorzFromNW;
		static const ReaderDir Rotate90 = ReaderDir::VertFromSW;
		static const ReaderDir Rotate180 = ReaderDir::HorzFromSE;
		static const ReaderDir Rotate270 = ReaderDir::VertFromNE;
		static const ReaderDir MirrorHorz = ReaderDir::HorzFromNE;
		static const ReaderDir MirrorVert = ReaderDir::HorzFromSW;
	};
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
		void MoveForward() {
			iCol_++;
			p_ += advancePerCol_;
			if (iCol_ == nCols_) {
				iCol_ = 0, iRow_++;
				pRow_ += advancePerRow_, p_ = pRow_;
			}
		}
		bool HasDone() const { return iRow_ >= nRows_; }
	};
	template<typename T_Read> class Reader : public Sequencer {
	public:
		static Reader HorzFromNW(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x, y), width, height, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Reader HorzFromNE(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x + width - 1, y), width, height, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Reader HorzFromSW(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x, y + height - 1), width, height, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Reader HorzFromSE(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x + width - 1, y + height - 1), width, height, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Reader VertFromNW(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x, y), width, height, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Reader VertFromNE(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x + width - 1, y), width, height, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Reader VertFromSW(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x, y + height - 1), width, height, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Reader VertFromSE(const Image& image, int x, int y, int width, int height) {
			return Reader(image.GetPointer(x + width - 1, y + height - 1), width, height, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Reader Create(const Image& image, int x, int y, int width, int height, ReaderDir dir) {
			switch (dir) {
			case ReaderDir::HorzFromNW: return HorzFromNW(image, x, y, width, height);
			case ReaderDir::HorzFromNE: return HorzFromNE(image, x, y, width, height);
			case ReaderDir::HorzFromSW: return HorzFromSW(image, x, y, width, height);
			case ReaderDir::HorzFromSE: return HorzFromSE(image, x, y, width, height);
			case ReaderDir::VertFromNW: return VertFromNW(image, x, y, width, height);
			case ReaderDir::VertFromNE: return VertFromNE(image, x, y, width, height);
			case ReaderDir::VertFromSW: return VertFromSW(image, x, y, width, height);
			case ReaderDir::VertFromSE: return VertFromSE(image, x, y, width, height);
			default: break;
			}
			return Reader(nullptr, 0, 0, 0, 0);
		}
	public:
		Reader(const void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				Sequencer(const_cast<void*>(p), nCols, nRows, advancePerCol, advancePerRow) {}
		typename T_Read::Type ReadForward() {
			auto rtn  = T_Read()(p_);
			MoveForward();
			return rtn;
		}
	};
	template<typename T_Write> class Writer : public Sequencer {
		static Writer HorzFromNW(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x, y), width, height, image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Writer HorzFromNE(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x + width - 1, y), width, height, -image.GetBytesPerPixel(), image.GetBytesPerLine());
		}
		static Writer HorzFromSW(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x, y + height - 1), width, height, image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Writer HorzFromSE(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x + width - 1, y + height - 1), width, height, -image.GetBytesPerPixel(), -image.GetBytesPerLine());
		}
		static Writer VertFromNW(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x, y), width, height, image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Writer VertFromNE(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x + width - 1, y), width, height, image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Writer VertFromSW(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x, y + height - 1), width, height, -image.GetBytesPerLine(), image.GetBytesPerPixel());
		}
		static Writer VertFromSE(const Image& image, int x, int y, int width, int height) {
			return Writer(image.GetPointer(x + width - 1, y + height - 1), width, height, -image.GetBytesPerLine(), -image.GetBytesPerPixel());
		}
		static Writer Create(const Image& image, int x, int y, int width, int height, WriterDir dir) {
			switch (dir) {
			case WriterDir::HorzFromNW: return HorzFromNW(image, x, y, width, height);
			case WriterDir::HorzFromNE: return HorzFromNE(image, x, y, width, height);
			case WriterDir::HorzFromSW: return HorzFromSW(image, x, y, width, height);
			case WriterDir::HorzFromSE: return HorzFromSE(image, x, y, width, height);
			case WriterDir::VertFromNW: return VertFromNW(image, x, y, width, height);
			case WriterDir::VertFromNE: return VertFromNE(image, x, y, width, height);
			case WriterDir::VertFromSW: return VertFromSW(image, x, y, width, height);
			case WriterDir::VertFromSE: return VertFromSE(image, x, y, width, height);
			default: break;
			}
			return Writer(nullptr, 0, 0, 0, 0);
		}
	public:
		Writer(void* p, int nCols, int nRows, int advancePerCol, int advancePerRow) :
				Sequencer(p, nCols, nRows, advancePerCol, advancePerRow) {}
		void WriteForward(const typename T_Write::Type& data) {
			auto rtn  = T_Write()(p_, data);
			MoveForward();
		}
	};
public:
	struct Format {
		int bytesPerPixel;
	public:
		static const Format Gray;
		static const Format RGB;
		static const Format RGBA;
		static const Format RGB565;
	public:
		bool IsIdentical(const Format& format) const { return this == &format; }
	};
private:
	const Format& format_;
	int width_;
	int height_;
	int bytesPerLine_;
	uint8_t* data_;
	bool writableFlag_;
public:
	Image(const Format& format, int width, int height, const void* data = nullptr) :
			format_{format}, width_{width}, height_{height},
			bytesPerLine_{width * format_.bytesPerPixel},
			data_{reinterpret_cast<uint8_t*>(const_cast<void*>(data))}, writableFlag_{false} {}
	bool IsFormatGray() const { return format_.IsIdentical(Format::Gray); }
	bool IsFormatRGB() const { return format_.IsIdentical(Format::RGB); }
	bool IsFormatRGBA() const { return format_.IsIdentical(Format::RGBA); }
	bool IsFormatRGB565() const { return format_.IsIdentical(Format::RGB565); }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	int GetBytesPerPixel() const { return format_.bytesPerPixel; }
	int GetBytesPerLine() const { return bytesPerLine_; }
	uint8_t* GetPointer() { return data_; }
	const uint8_t* GetPointer() const { return data_; }
	const uint8_t* GetPointer(int x, int y) const {
		return data_ + GetBytesPerPixel() * x + GetBytesPerLine() * y;
	}
	bool IsWritable() const { return writableFlag_; }
	static bool IsDirHorz(SequencerDir dir) { return dir < SequencerDir::Vert; }
	static bool IsDirVert(SequencerDir dir) { return dir >= SequencerDir::Vert; }
};

}

#endif
