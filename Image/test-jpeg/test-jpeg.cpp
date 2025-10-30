#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "jxglib/FS.h"

using namespace jxglib;

void write_to_stream(void* context, void* data, int size)
{
	Stream* pStream = reinterpret_cast<Stream*>(context);
	pStream->Write(data, size);
}

int my_read(void* user, char* data, int size)
{
	FS::File* pFile = static_cast<FS::File*>(user);
	return pFile->Read(data, size); // 読み込んだバイト数を返す
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
int main()
{
	stdio_init_all();
	Stream* pStream;
	int x, y, channels;
	::stbi_load_from_callbacks(nullptr, pStream, &x, &y, &channels, 0);
	::stbi_write_bmp_to_func(write_to_stream, pStream, 0, 0, 0, nullptr);
	::stbi_write_png_to_func(write_to_stream, pStream, 0, 0, 0, nullptr, 0);
	::stbi_write_jpg_to_func(write_to_stream, pStream, 0, 0, 0, nullptr, 90);
	while (true) {
		printf("Hello, world!\n");
		sleep_ms(1000);
	}
}
