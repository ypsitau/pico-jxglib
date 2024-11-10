//==============================================================================
// jxglib/Image.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGE_H
#define PICO_JXGLIB_IMAGE_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Image
//------------------------------------------------------------------------------
class Image {
public:
	struct Format {
		int bytesPerPixel;
	public:
		static const Format Gray;
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
			format_{format}, width_{width}, height_{height}, bytesPerLine_{width * format.bytesPerPixel},
			data_{reinterpret_cast<uint8_t*>(const_cast<void*>(data))}, writableFlag_{false} {}
	bool IsFormatGray() const { return format_.IsIdentical(Format::Gray); }
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
};

}

#endif
