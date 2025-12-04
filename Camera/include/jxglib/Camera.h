//==============================================================================
// jxglib/Camera.h
//==============================================================================
#ifndef PICO_JXGLIB_CAMERA_H
#define PICO_JXGLIB_CAMERA_H
#include "pico/stdlib.h"
#include "jxglib/Image.h"

namespace jxglib::Camera {

//------------------------------------------------------------------------------
// Camera
//------------------------------------------------------------------------------
class Base {
private:
	static Base* pHead_;
private:
	Base* pNext_;
public:
	Base(bool registerFlag = true);
	virtual ~Base();
public:
	bool IsValid() const;
public:
	static Base* GetHead() { return pHead_; }
	Base* GetNext() { return pNext_; }
	const Base* GetNext() const { return pNext_; }
public:
	virtual const Image& Capture() = 0;
	virtual const char* GetName() const { return "no-name"; }
	virtual const char* GetRemarks(char* buff, int lenMax) const { buff[0] = '\0'; return buff;}
};

//------------------------------------------------------------------------------
// Dummy
//------------------------------------------------------------------------------
class Dummy : public Base {
public:
	Dummy() : Base(false) {}
public:
	static Dummy Instance;
public:
	virtual const Image& Capture() override { return Image::None; }
	virtual const char* GetName() const override { return "Dummy"; }
};

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
Base& GetInstance(int iCamera);

}

#endif
