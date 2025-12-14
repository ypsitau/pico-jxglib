//==============================================================================
// jxglib/Camera.h
//==============================================================================
#ifndef PICO_JXGLIB_CAMERA_H
#define PICO_JXGLIB_CAMERA_H
#include "pico/stdlib.h"
#include "jxglib/Image.h"

namespace jxglib::Camera {

enum class Resolution { None, VGA, QVGA, QQVGA, QQQVGA, CIF, QCIF, QQCIF };
enum class Format { None, RawBayerRGB, ProcessedBayerRGB, YUV422, GRB422, RGB565, RGB555, RGB444 };

//------------------------------------------------------------------------------
// Camera::Base
//------------------------------------------------------------------------------
class Base {
private:
	static Base* pHead_;
private:
	Base* pNext_;
	bool capturedFlag_;
	int64_t usecPerFrame_;
	absolute_time_t timeLastCapture_;
protected:
	Size size_;
	Resolution resolution_;
	Format format_;
public:
	Base(bool registerFlag = true);
	virtual ~Base();
public:
	bool IsValid() const;
public:
	static Base* GetHead() { return pHead_; }
	Base* GetNext() { return pNext_; }
	const Base* GetNext() const { return pNext_; }
	const Image& Capture();
	int GetFPS() const { return (usecPerFrame_ > 0)? static_cast<int>(1000000 / usecPerFrame_) : 0; }
public:
	const Size& GetSize() const { return size_; }
	void UpdateSize();
	Resolution GetResolution() const { return resolution_; }
	Format GetFormat() const { return format_; }
public:
	virtual Base& SetResolution(Resolution resolution);
	virtual Base& SetFormat(Format format) { format_ = format; return *this; }
	virtual void FreeResource() {}
	virtual void DoCapture() = 0;
	virtual const Image& GetImage() const = 0;
	virtual const char* GetName() const { return "no-name"; }
	virtual const char* GetRemarks(char* buff, int lenMax) const { buff[0] = '\0'; return buff;}
};

//------------------------------------------------------------------------------
// Camera::Dummy
//------------------------------------------------------------------------------
class Dummy : public Base {
public:
	Dummy() : Base(false) {}
public:
	static Dummy Instance;
public:
	virtual const Image& GetImage() const override { return Image::None; }
	virtual void DoCapture() override {}
	virtual const char* GetName() const override { return "Dummy"; }
};

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
Base& GetInstance(int iCamera);

}

#endif
