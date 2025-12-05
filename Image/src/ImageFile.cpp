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
	int width = image.GetWidth();
	int height = image.GetHeight();
	if (image.GetFormat().IsRGB565()) {
		if (format == Format::BMP) {
			const int headerSize = 14 + 40; // BITMAPFILEHEADER + BITMAPINFOHEADER
			const int rowSize = ((width * 2 + 3) / 4) * 4;
			const int imageSize = rowSize * height;
			const int fileSize = headerSize + imageSize;
			uint8_t header[headerSize];
			memset(header, 0, headerSize);
			// BITMAPFILEHEADER
			header[0] = 'B'; header[1] = 'M';
			*reinterpret_cast<uint32_t*>(&header[2]) = fileSize;
			*reinterpret_cast<uint32_t*>(&header[10]) = headerSize;
			// BITMAPINFOHEADER
			*reinterpret_cast<uint32_t*>(&header[14]) = 40;
			*reinterpret_cast<int32_t*>(&header[18]) = width;
			*reinterpret_cast<int32_t*>(&header[22]) = height;
			*reinterpret_cast<uint16_t*>(&header[26]) = 1; // plane count 
			*reinterpret_cast<uint16_t*>(&header[28]) = 16; // bits per pixel
			*reinterpret_cast<uint32_t*>(&header[30]) = 3; // BI_BITFIELDS
			*reinterpret_cast<uint32_t*>(&header[34]) = imageSize;
			file.Write(header, headerSize);
			uint32_t masks[3] = {0xF800, 0x07E0, 0x001F}; // R, G, B
			//uint32_t masks[3] = {0x001F, 0x07E0, 0xF800}; // B, G, R
			file.Write(masks, sizeof(masks));
			const uint16_t* src = reinterpret_cast<const uint16_t*>(image.GetPointer());
			uint8_t* rowBuffer = new uint8_t[rowSize];
			for (int y = height - 1; y >= 0; y--) {
				memcpy(rowBuffer, &src[y * width], width * 2);
				if (rowSize > width * 2) {
					memset(&rowBuffer[width * 2], 0, rowSize - width * 2);
				}
				for (int x = 0; x < width; x++) {
					uint8_t byte1 = rowBuffer[x * 2 + 1];
					uint8_t byte2 = rowBuffer[x * 2 + 0];
					rowBuffer[x * 2 + 0] = byte1;
					rowBuffer[x * 2 + 1] = byte2;
				}
				file.Write(rowBuffer, rowSize);
			}
			delete[] rowBuffer;
			return true;
		} else {
			return false;
		}
	}
	int result = 0;
	switch (format) {
	case Format::BMP: {
		result = ::stbi_write_bmp_to_func(WriterCB::write, &file,
			width, height,
			image.GetFormat().IsRGBA()? 4 : 3,
			image.GetPointer());
		break;
	}
	case Format::PNG: {
		result = ::stbi_write_png_to_func(WriterCB::write, &file,
			width, height,
			image.GetFormat().IsRGBA()? 4 : 3,
			image.GetPointer(), image.GetBytesPerLine());
		break;
	}
	case Format::JPEG:
		result = ::stbi_write_jpg_to_func(WriterCB::write, &file,
			width, height,
			image.GetFormat().IsRGBA()? 4 : 3,
			image.GetPointer(), 90);
		break;
	default:
		break;
	}
	return result != 0;
}

}
