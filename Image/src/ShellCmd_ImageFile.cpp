#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/ImageFile.h"
#include "jxglib/Display.h"

namespace jxglib::ShellCmd_ImageFile {

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
	if (!ImageFile::Read(image, *pFile)) {
		printf("Failed to read image file: %s\n", fileName);
		return Result::Error;
	}
	Display::GetInstance(0).DrawImage(0, 0, image, Rect(0, 0, 16, 16)).Refresh();
	return Result::Success;
}

}
