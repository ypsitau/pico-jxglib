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
			const int bytesHeader = BitmapFileHeader::bytes + BitmapV3InfoHeader::bytes;
			const int bytesRow = ((width * 2 + 3) / 4) * 4;
			const int bytesPadding = bytesRow - (width * 2);
			const int bytesImage = bytesRow * height;
			const int bytesFile = bytesHeader + bytesImage;
			const uint8_t padding[3] = {0, 0, 0};
			BitmapFileHeader bfh;
			BitmapV3InfoHeader bih;
			Pack_uint16(bfh.bfType, 0x4d42); // 'BM'
			Pack_uint32(bfh.bfSize, bytesFile);
			Pack_uint16(bfh.bfReserved1, 0);
			Pack_uint16(bfh.bfReserved2, 0);
			Pack_uint32(bfh.bfOffBits, bytesHeader);
			Pack_uint32(bih.biSize, BitmapV3InfoHeader::bytes);
			Pack_int32(bih.biWidth, width);
			Pack_int32(bih.biHeight, height);
			Pack_uint16(bih.biPlanes, 1);
			Pack_uint16(bih.biBitCount, 16);
			Pack_uint32(bih.biCompression, 3); // BI_BITFIELDS
			Pack_uint32(bih.biSizeImage, bytesImage);
			Pack_int32(bih.biXPelsPerMeter, 2835);
			Pack_int32(bih.biYPelsPerMeter, 2835);
			Pack_uint32(bih.biClrUsed, 0);
			Pack_uint32(bih.biClrImportant, 0);
			Pack_uint32(bih.biRedMask,	0b1111100000000000);
			Pack_uint32(bih.biGreenMask,0b0000011111100000);
			Pack_uint32(bih.biBlueMask,	0b0000000000011111);
			Pack_uint32(bih.biAlphaMask, 0x0000);
			file.Write(&bfh, BitmapFileHeader::bytes);
			file.Write(&bih, BitmapV3InfoHeader::bytes);
			const uint16_t* src = reinterpret_cast<const uint16_t*>(image.GetPointer());
			for (int y = height - 1; y >= 0; y--) {
				file.Write(&src[y * width], width * 2);
				if (bytesPadding > 0) file.Write(padding, bytesPadding);
			}
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
