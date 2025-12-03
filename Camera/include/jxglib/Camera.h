//==============================================================================
// jxglib/Camera.h
//==============================================================================
#ifndef PICO_JXGLIB_CAMERA_H
#define PICO_JXGLIB_CAMERA_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

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
	Base();
	virtual ~Base();
public:
	static Base* GetHead() { return pHead_; }
	Base* GetNext() { return pNext_; }
	const Base* GetNext() const { return pNext_; }
public:
	virtual bool IsDummy() const { return false; }
	virtual const char* GetName() const { return "no-name"; }
	virtual const char* GetRemarks(char* buff, int lenMax) const { buff[0] = '\0'; return buff;}
};

//------------------------------------------------------------------------------
// Dummy
//------------------------------------------------------------------------------
class Dummy : public Base {
public:
	Dummy() {}
public:
	static Dummy Instance;
public:
	virtual bool IsDummy() const override { return true; }
	virtual const char* GetName() const override { return "Dummy"; }
};

}

#endif
