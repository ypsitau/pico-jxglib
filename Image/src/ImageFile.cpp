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
// functions
//------------------------------------------------------------------------------
bool Read(Image& image, FS::File& file)
{
	struct ReaderCB {
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
	stbi_io_callbacks callbacks { read: ReaderCB::read, skip: ReaderCB::skip, eof: ReaderCB::eof };
	int x, y, channels;
	const Image::Format& format = image.GetFormat();
	unsigned char* data = ::stbi_load_from_callbacks(&callbacks, &file, &x, &y, &channels,
						format.IsGray()? 1 : format.IsRGB()? 3 : format.IsRGBA()? 4 : 0);
	if (!data) return false;
	image.SetMemory(!format.IsNone()? format :
		(channels == 1)? Image::Format::Gray : (channels == 3)? Image::Format::RGB : Image::Format::RGBA,
		x, y, new MemoryStbi(data));
	return true;
}

bool Write(Image& image, FS::File& file, Format format)
{
	struct WriterCB {
		static void write(void* user, void* data, int size) {
			FS::File* pFile = static_cast<FS::File*>(user);
			pFile->Write(data, size);
		}
	};
	int result = 0;
	switch (format) {
	case Format::BMP: {
		result = ::stbi_write_bmp_to_func(WriterCB::write, &file,
			image.GetWidth(), image.GetHeight(),
			image.GetFormat().IsRGBA()? 4 : 3,
			image.GetPointer());
		break;
	}
	case Format::PNG: {
		result = ::stbi_write_png_to_func(WriterCB::write, &file,
			image.GetWidth(), image.GetHeight(),
			image.GetFormat().IsRGBA()? 4 : 3,
			image.GetPointer(), image.GetBytesPerLine());
		break;
	}
	case Format::JPEG:
		result = ::stbi_write_jpg_to_func(WriterCB::write, &file,
			image.GetWidth(), image.GetHeight(),
			image.GetFormat().IsRGBA()? 4 : 3,
			image.GetPointer(), 90);
		break;
	default:
		break;
	}
	return (result != 0);
}

}
