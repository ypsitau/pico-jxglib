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
	enum Format { Bitmap, Gray, RGB, BGR, RGBA, BGRA, RGB565 };
private:
	Format format_;
	int width_;
	int height_;
	uint8_t* data_;
	bool writableFlag_;
public:
	Image(Format format, int width, int height, const void* data = nullptr) :
			format_{format}, width_{width}, height_{height},
			data_{reinterpret_cast<uint8_t*>(const_cast<void*>(data))}, writableFlag_{false} {}
	Format GetFormat() const { return format_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	const uint8_t* GetData() const { return data_; }
	bool IsWritable() const { return writableFlag_; }
};

}

#endif
