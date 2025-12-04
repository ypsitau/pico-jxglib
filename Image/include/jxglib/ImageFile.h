//==============================================================================
// jxglib/ImageFile.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGEFILE_H
#define PICO_JXGLIB_IMAGEFILE_H
#include "jxglib/Image.h"
#include "jxglib/FS.h"

namespace jxglib::ImageFile {

enum class Format { BMP, PNG, JPEG };

bool Read(Image& image, FS::File& file);
bool Write(Image& image, FS::File& file, Format format);

}

#endif
