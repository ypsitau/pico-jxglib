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
	enum Format { RGB, BGR, RGBA, BGRA, RGB565 };
private:
	Format format_;
	int width_;
	int height_;
	uint8_t* buff_;
	bool writableFlag_;
public:
	Image(Format format, int width, int height, const void* buff = nullptr) :
			format_{format}, width_{width}, height_{height},
			buff_{reinterpret_cast<uint8_t*>(const_cast<void*>(buff))}, writableFlag_{false} {}
};

}

#endif
