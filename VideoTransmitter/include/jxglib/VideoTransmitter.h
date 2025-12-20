//==============================================================================
// jxglib/VideoTransmitter.h
//==============================================================================
#ifndef PICO_JXGLIB_VIDEOTRANSMITTER_H
#define PICO_JXGLIB_VIDEOTRANSMITTER_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VideoTransmitter
//------------------------------------------------------------------------------
class VideoTransmitter {
private:
	static VideoTransmitter* pHead_;
private:
	VideoTransmitter* pNext_;
public:
	VideoTransmitter();
	virtual ~VideoTransmitter();
public:
	static VideoTransmitter* GetHead() { return pHead_; }
	VideoTransmitter* GetNext() { return pNext_; }
	const VideoTransmitter* GetNext() const { return pNext_; }
public:
	bool IsValid() const;
public:
	int GetWidth() const { return GetSize().width; }
	int GetHeight() const { return GetSize().height; }
public:
	virtual const Size& GetSize() const = 0;
	virtual void Initialize() = 0;
	virtual bool CanTransmit() = 0;
	virtual void Transmit(const void* buffFrame) = 0;
public:
	virtual const char* GetName() const { return "no-name"; }
public:
	static VideoTransmitter& GetInstance(int iVideoTransmitter);
};

//------------------------------------------------------------------------------
// VideoTransmitterDummy
//------------------------------------------------------------------------------
class VideoTransmitterDummy : public VideoTransmitter {
public:
	VideoTransmitterDummy() {}
public:
	static VideoTransmitterDummy Instance;
public:
	virtual const Size& GetSize() const override { return Size::Zero; }
	virtual void Initialize() override {}
	virtual bool CanTransmit() override { return false; }
	virtual void Transmit(const void* buffFrame) override {}
public:
	virtual const char* GetName() const override { return "VideoTransmitterDummy"; }
};

}

#endif
