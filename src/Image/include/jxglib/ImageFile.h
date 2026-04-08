//==============================================================================
// jxglib/ImageFile.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGEFILE_H
#define PICO_JXGLIB_IMAGEFILE_H
#include "jxglib/Image.h"
#include "jxglib/FS.h"
#include "jxglib/PackedNumber.h"

namespace jxglib::ImageFile {

//------------------------------------------------------------------------------
// BitmapFileHeader
//------------------------------------------------------------------------------
struct BitmapFileHeader {
	Packed_uint16(bfType);
	Packed_uint32(bfSize);
	Packed_uint16(bfReserved1);
	Packed_uint16(bfReserved2);
	Packed_uint32(bfOffBits);
	static const size_t bytes = 14;
};

//------------------------------------------------------------------------------
// BitmapInfoHeader
//------------------------------------------------------------------------------
struct BitmapInfoHeader {
	Packed_uint32(biSize);
	Packed_int32(biWidth);
	Packed_int32(biHeight);
	Packed_uint16(biPlanes);
	Packed_uint16(biBitCount);
	Packed_uint32(biCompression);
	Packed_uint32(biSizeImage);
	Packed_int32(biXPelsPerMeter);
	Packed_int32(biYPelsPerMeter);
	Packed_uint32(biClrUsed);
	Packed_uint32(biClrImportant);
	static const size_t bytes = 40;
};

//------------------------------------------------------------------------------
// BitmapV3InfoHeader
//------------------------------------------------------------------------------
struct BitmapV3InfoHeader {
	Packed_uint32(biSize);
	Packed_int32(biWidth);
	Packed_int32(biHeight);
	Packed_uint16(biPlanes);
	Packed_uint16(biBitCount);
	Packed_uint32(biCompression);
	Packed_uint32(biSizeImage);
	Packed_int32(biXPelsPerMeter);
	Packed_int32(biYPelsPerMeter);
	Packed_uint32(biClrUsed);
	Packed_uint32(biClrImportant);
    Packed_uint32(biRedMask);
    Packed_uint32(biGreenMask);
    Packed_uint32(biBlueMask);
    Packed_uint32(biAlphaMask);
	static const size_t bytes = 56;
};

//------------------------------------------------------------------------------
// Format
//------------------------------------------------------------------------------
enum class Format { BMP, PNG, JPEG };

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
bool Read(Image& image, FS::File& file);
bool Write(Image& image, FS::File& file, Format format);

}

#endif
