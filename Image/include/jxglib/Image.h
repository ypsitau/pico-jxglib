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
	enum Format { Bitmap, RGB, BGR, RGBA, BGRA, RGB565 };
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
};

}

#endif
