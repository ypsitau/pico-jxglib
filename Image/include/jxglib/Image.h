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
	enum class Format { Gray, RGB565 };
private:
	Format format_;
	int width_;
	int height_;
	int bytesPerPixel_;
	int bytesPerLine_;
	uint8_t* data_;
	bool writableFlag_;
public:
	Image(Format format, int width, int height, const void* data = nullptr) :
			format_{format}, width_{width}, height_{height},
			bytesPerPixel_{(format_ == Format::Gray)? 1 : (format_ == Format::RGB565)? 2 : 3},
			bytesPerLine_{width * bytesPerPixel_},
			data_{reinterpret_cast<uint8_t*>(const_cast<void*>(data))}, writableFlag_{false} {}
	bool IsFormatGray() const { return format_ == Format::Gray; }
	bool IsFormatRGB565() const { return format_ ==  Format::RGB565; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	int GetBytesPerPixel() const { return bytesPerPixel_; }
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
