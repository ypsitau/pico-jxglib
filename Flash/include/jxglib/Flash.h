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
		uint32_t offsetXIPStart_;
		uint32_t offsetXIPCached_;
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
	struct Param_Erase { uint32_t offsetXIP; uint32_t bytes; };
	struct Param_Program { uint32_t offsetXIP; const void* data; uint32_t bytes; };
protected:
	uint32_t offsetXIPCached_;
	uint8_t buffCache_[SectorSize];
public:
	static Flash Instance;
public:
	Flash() : offsetXIPCached_{UINT32_MAX} {}
public:
	static void Read(uint32_t offsetXIP, void* buff, uint32_t bytes) { Instance.Read_(offsetXIP, buff, bytes); }
	static void Write(uint32_t offsetXIP, const void* buff, uint32_t bytes) { Instance.Write_(offsetXIP, buff, bytes); }
	static void Synchronize() { Instance.Synchronize_(); }
protected:
	void Read_(uint32_t offsetXIP, void* buff, uint32_t bytes);
	void Write_(uint32_t offsetXIP, const void* buff, uint32_t bytes);
	void Synchronize_();
public:
	static uint32_t GetAddress(uint32_t offsetXIP) { return XIP_BASE + offsetXIP; }
	template<typename T = void>
	static const T* GetPointerXIP(uint32_t offsetXIP) { return reinterpret_cast<const T*>(XIP_BASE + offsetXIP); }
	static void EraseUnsafe(uint32_t offsetXIP, uint32_t bytes) {
		::flash_range_erase(offsetXIP, bytes);
	}
	static void ProgramUnsafe(uint32_t offsetXIP, const void* data, uint32_t bytes) {
		::flash_range_program(offsetXIP, reinterpret_cast<const uint8_t*>(data), bytes);
	}
	void EraseAndProgram(uint32_t offsetXIP, const void* data, uint32_t bytes) {
		Erase(offsetXIP, bytes);
		Program(offsetXIP, data, bytes);
	}
	virtual void Erase(uint32_t offsetXIP, uint32_t bytes) {
		Param_Erase param { offsetXIP, bytes };
		::flash_safe_execute(Erase_, &param, Timeout);
	}
	virtual void Program(uint32_t offsetXIP, const void* data, uint32_t bytes) {
		Param_Program param { offsetXIP, data, bytes };
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
		EraseUnsafe(paramEx.offsetXIP, paramEx.bytes);
	}
	static void Program_(void* param) {
		auto& paramEx = *reinterpret_cast<Param_Program*>(param);
		ProgramUnsafe(paramEx.offsetXIP, paramEx.data, paramEx.bytes);
	}
};

}

#endif
