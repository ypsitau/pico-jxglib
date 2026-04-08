//==============================================================================
// jxglib/FixedPool.h
//==============================================================================
#ifndef PICO_JXGLIB_FIXEDPOOL_H
#define PICO_JXGLIB_FIXEDPOOL_H
#include "pico/stdlib.h"

#define FixedPoolAllocator(inst, ownerName) \
static void *operator new(size_t bytes) { return inst.Allocate(ownerName, bytes); } \
static void operator delete(void* p) { inst.Free(p); }

namespace jxglib {

//-----------------------------------------------------------------------------
// FixedPool
//-----------------------------------------------------------------------------
class FixedPool {
public:
	struct Header {
		union {
			uint8_t* p;
			Header* pHeaderNext;
			const char* ownerName;
		};
	};
private:
	const char* name_;
	uint8_t* buff_;
	int bytesBlock_;
	int nBlocks_;
	int iBlockLast_;
	Header* pHeaderFreedTop_;
public:
	FixedPool(const char* name, int bytesBlock, int nBlocks);
public:
	const char* GetName() { return name_; }
	void Initialize();
	FixedPool& SetBlocks(int nBlocks) { nBlocks_ = nBlocks; return *this; }
	void* Allocate(const char* ownerName, int bytes);
	void Free(void* p);
	bool IsUsed(const Header* pHeader) const;
	int GetBytesBuff() const { return nBlocks_ * (sizeof(Header) + bytesBlock_); }
public:
	void PrintUsage() const;
};
		
}

#endif
