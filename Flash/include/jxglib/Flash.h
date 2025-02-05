//==============================================================================
// jxglib/Flash.h
//==============================================================================
#ifndef PICO_JXGLIB_FLASH_H
#define PICO_JXGLIB_FLASH_H
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
		uint32_t offset_;
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
	struct Param_Erase { uint32_t offset; size_t bytes; };
	struct Param_Program { uint32_t offset; const void* data; size_t bytes; };
public:
	Flash() {}
public:
	static uint32_t GetAddress(uint32_t offset) { return XIP_BASE + offset; }
	template<typename T = void>
	static const T* GetPointer(uint32_t offset) { return reinterpret_cast<const T*>(XIP_BASE + offset); }
	static void Erase(uint32_t offset, size_t bytes) {
		::flash_range_erase(offset, bytes);
	}
	static void EraseSafe(uint32_t offset, size_t bytes) {
		Param_Erase param { offset, bytes };
		::flash_safe_execute(EraseSafe_, &param, Timeout);
	}
	static void Program(uint32_t offset, const void* data, size_t bytes) {
		::flash_range_program(offset, reinterpret_cast<const uint8_t*>(data), bytes);
	}
	static void ProgramSafe(uint32_t offset, const void* data, size_t bytes) {
		Param_Program param { offset, data, bytes };
		::flash_safe_execute(ProgramSafe_, &param, Timeout);
	}
private:
	static void EraseSafe_(void* param) {
		auto& paramEx = *reinterpret_cast<Param_Erase*>(param);
		Erase(paramEx.offset, paramEx.bytes);
	}
	static void ProgramSafe_(void* param) {
		auto& paramEx = *reinterpret_cast<Param_Program*>(param);
		Program(paramEx.offset, paramEx.data, paramEx.bytes);
	}
};

}

#endif
