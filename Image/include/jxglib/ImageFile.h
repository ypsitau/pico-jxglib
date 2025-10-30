//==============================================================================
// jxglib/ImageFile.h
//==============================================================================
#ifndef PICO_JXGLIB_IMAGEFILE_H
#define PICO_JXGLIB_IMAGEFILE_H
#include "jxglib/Image.h"
#include "jxglib/FS.h"

namespace jxglib::ImageFile {

bool Read(Image& image, FS::File& file);

}

#endif
