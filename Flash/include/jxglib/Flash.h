//==============================================================================
// jxglib/Flash.h
//==============================================================================
#ifndef PICO_JXGLIB_FLASH_H
#define PICO_JXGLIB_FLASH_H
#include <memory.h>
#include "pico/stdlib.h"
#include "pico/flash.h"
#include "hardware/flash.h"
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Flash
//------------------------------------------------------------------------------
class Flash {
public:
	static const uint32_t Timeout = UINT32_MAX;
	static const uint32_t Address = XIP_BASE;
	static const uint32_t BlockSize = FLASH_BLOCK_SIZE;
	static const uint32_t SectorSize = FLASH_SECTOR_SIZE;
	static const uint32_t PageSize = FLASH_PAGE_SIZE;
public:
	class Stream : public jxglib::Stream {
	private:
		uint32_t offsetStart_;
		uint32_t offsetCached_;
		uint32_t bytesBuffPage_;
		uint8_t buffPage_[PageSize];
	public:
		Stream(uint32_t offset);
	public:
		virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) override;
		virtual bool Write(const void* buff, int bytesBuff) override;
		void Flush();
	};
public:
	struct Param_Erase { uint32_t offset; uint32_t bytes; };
	struct Param_Program { uint32_t offset; const void* data; uint32_t bytes; };
protected:
	uint32_t offsetCached_;
	uint8_t buffCache_[SectorSize];
public:
	static Flash Instance;
public:
	Flash() : offsetCached_{UINT32_MAX} {}
public:
	static void Read(uint32_t offset, void* buff, uint32_t bytes) { Instance.Read_(offset, buff, bytes); }
	static void Write(uint32_t offset, const void* buff, uint32_t bytes) { Instance.Write_(offset, buff, bytes); }
protected:
	void Read_(uint32_t offset, void* buff, uint32_t bytes);
	void Write_(uint32_t offset, const void* buff, uint32_t bytes);
public:
	static uint32_t GetAddress(uint32_t offset) { return XIP_BASE + offset; }
	template<typename T = void>
	static const T* GetPointerXIP(uint32_t offset) { return reinterpret_cast<const T*>(XIP_BASE + offset); }
	static void EraseUnsafe(uint32_t offset, uint32_t bytes) {
		::flash_range_erase(offset, bytes);
	}
	static void ProgramUnsafe(uint32_t offset, const void* data, uint32_t bytes) {
		::flash_range_program(offset, reinterpret_cast<const uint8_t*>(data), bytes);
	}
	virtual void Erase(uint32_t offset, uint32_t bytes) {
		Param_Erase param { offset, bytes };
		::flash_safe_execute(Erase_, &param, Timeout);
	}
	virtual void Program(uint32_t offset, const void* data, uint32_t bytes) {
		Param_Program param { offset, data, bytes };
		::flash_safe_execute(Program_, &param, Timeout);
	}
	virtual void CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes) {
		::memcpy(reinterpret_cast<uint8_t*>(dst) + offsetDst, reinterpret_cast<const uint8_t*>(src) + offsetSrc, bytes);
	}
	void CopyMemory(void* dst, uint32_t offsetDst, uint32_t src, uint32_t offsetSrc, uint32_t bytes) {
		CopyMemory(dst, offsetDst, reinterpret_cast<const void*>(src), offsetSrc, bytes);
	}
private:
	static void Erase_(void* param) {
		auto& paramEx = *reinterpret_cast<Param_Erase*>(param);
		EraseUnsafe(paramEx.offset, paramEx.bytes);
	}
	static void Program_(void* param) {
		auto& paramEx = *reinterpret_cast<Param_Program*>(param);
		ProgramUnsafe(paramEx.offset, paramEx.data, paramEx.bytes);
	}
public:
	static void Test();
};

class FlashDummy : public Flash {
public:
	void Read(uint32_t offset, void* buff, uint32_t bytes);
	void Write(uint32_t offset, const void* buff, uint32_t bytes);
public:
	virtual void Erase(uint32_t offset, uint32_t bytes) override;
	virtual void Program(uint32_t offset, const void* data, uint32_t bytes) override;
	virtual void CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes) override;
};

}

#endif
