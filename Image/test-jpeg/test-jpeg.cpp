#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "jxglib/FS.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

//------------------------------------------------------------------------------
// MemoryStbi
//------------------------------------------------------------------------------
class MemoryStbi : public Memory {
public:
	MemoryStbi(void* data) : Memory(data) {}
public:
	virtual void Free() override { ::stbi_image_free(data_); data_ = nullptr; }
};

void write_to_stream(void* context, void* data, int size)
{
	Stream* pStream = reinterpret_cast<Stream*>(context);
	pStream->Write(data, size);
}

int my_read(void* user, char* data, int size)
{
	FS::File* pFile = static_cast<FS::File*>(user);
	return pFile->Read(data, size);
}

void my_skip(void* user, int n)
{
	FS::File* pFile = static_cast<FS::File*>(user);
	pFile->Seek(n, FS::SeekStart::Current);
}

int my_eof(void* user)
{
	FS::File* pFile = static_cast<FS::File*>(user);
	return pFile->IsEOF();
}

ShellCmd_Named(show_image, "show-image", "show image file")
{
	if (argc < 2) {
		printf("Usage: show-image <image-file>\n");
		return Result::Error;
	}
	const char* fileName = argv[1];
	std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
	if (!pFile) {
		printf("Failed to open file: %s\n", fileName);
		return Result::Error;
	}
	stbi_io_callbacks callbacks;
	callbacks.read = my_read;
	callbacks.skip = my_skip;
	callbacks.eof  = my_eof;
	int x, y, channels;
	unsigned char* data = ::stbi_load_from_callbacks(&callbacks, pFile.get(), &x, &y, &channels, 0);
	if (!data) {
		printf("Failed to load image: %s\n", fileName);
		return Result::Error;
	}
	printf("Image loaded: %s (width=%d, height=%d, channels=%d)\n", fileName, x, y, channels);
	Image image;
	image.SetMemory((channels == 1)? Image::Format::Gray : (channels == 3)? Image::Format::RGB : Image::Format::RGBA, x, y, new MemoryStbi(data));
	return Result::Success;
}

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(false);
	//Stream* pStream;
	//int x, y, channels;
	//::stbi_load_from_callbacks(nullptr, pStream, &x, &y, &channels, 0);
	//::stbi_write_bmp_to_func(write_to_stream, pStream, 0, 0, 0, nullptr);
	//::stbi_write_png_to_func(write_to_stream, pStream, 0, 0, 0, nullptr, 0);
	//::stbi_write_jpg_to_func(write_to_stream, pStream, 0, 0, 0, nullptr, 90);
	while (true) ::jxglib_tick();
}
