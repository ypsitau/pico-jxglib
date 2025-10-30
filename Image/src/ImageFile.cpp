//==============================================================================
// ImageFile.cpp
//==============================================================================
#include "jxglib/ImageFile.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace jxglib::ImageFile {

//------------------------------------------------------------------------------
// MemoryStbi
//------------------------------------------------------------------------------
class MemoryStbi : public Memory {
public:
	MemoryStbi(void* data) : Memory(data) {}
public:
	virtual void Free() override { ::stbi_image_free(data_); data_ = nullptr; }
};

//------------------------------------------------------------------------------
// ReaderCB
//------------------------------------------------------------------------------
class ReaderCB {
public:
	static int read(void* user, char* data, int size) {
		FS::File* pFile = static_cast<FS::File*>(user);
		return pFile->Read(data, size);
	}
	static void skip(void* user, int n) {
		FS::File* pFile = static_cast<FS::File*>(user);
		pFile->Seek(n, FS::SeekStart::Current);
	}
	static int eof(void* user) {
		FS::File* pFile = static_cast<FS::File*>(user);
		return pFile->IsEOF();
	}
};

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
bool Read(Image& image, FS::File& file)
{
	stbi_io_callbacks callbacks;
	callbacks.read = ReaderCB::read;
	callbacks.skip = ReaderCB::skip;
	callbacks.eof  = ReaderCB::eof;
	int x, y, channels;
	const Image::Format& format = image.GetFormat();
	unsigned char* data = ::stbi_load_from_callbacks(&callbacks, &file, &x, &y, &channels,
						format.IsGray() ? 1 : format.IsRGB() ? 3 : format.IsRGBA() ? 4 : 0);
	if (!data) return false;
	image.SetMemory((channels == 1)? Image::Format::Gray : (channels == 3)? Image::Format::RGB : Image::Format::RGBA, x, y, new MemoryStbi(data));
	return true;
}

}
