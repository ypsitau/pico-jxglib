#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Image.h"
#include "jxglib/FS.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Display.h"

using namespace jxglib;

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
	Image image(Image::Format::RGB);
	Display::GetInstance(0).DrawImage(0, 0, image);
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
