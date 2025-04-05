#include <stdio.h>
#include <malloc.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/Stdio.h"

using namespace jxglib;

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
	uint8_t* buff_;
	int bytesBlock_;
	int nBlocks_;
	int iBlockLast_;
	Header* pHeaderFreedTop_;
public:
	static FixedPool Inst64;
public:
	FixedPool(int bytesBlock);
public:
	void Initialize(int nBlocks);
	void* Allocate(const char* ownerName, int bytes);
	void Free(void* p);
	bool IsUsed(const Header* pHeader) const;
	int GetBytesBuff() const { return nBlocks_ * (sizeof(Header) + bytesBlock_); }
public:
	void PrintUsage(Printable& printable) const;
};

FixedPool FixedPool::Inst64(64);

FixedPool::FixedPool(int bytesBlock) : buff_{nullptr}, bytesBlock_{bytesBlock}, nBlocks_{0}, pHeaderFreedTop_{nullptr}
{
}

void FixedPool::Initialize(int nBlocks)
{
	nBlocks_ = nBlocks;
	int bytesBuff = GetBytesBuff();
	buff_ = reinterpret_cast<uint8_t*>(::malloc(bytesBuff));
	if (!buff_) ::panic("failed to allocate memory in FixedPool::Initialize()");
	::memset(buff_, 0x00, bytesBuff);
}

void* FixedPool::Allocate(const char* ownerName, int bytes)
{
	Header* pHeader;
	if (bytes > bytesBlock_) {
		::panic("the size of %s is %d bytes, which is over %d bytes", bytes, bytesBlock_);
	} else if (pHeaderFreedTop_) {
		pHeader = pHeaderFreedTop_;
		pHeaderFreedTop_ = pHeaderFreedTop_->pHeaderNext;
	} else if (iBlockLast_ < nBlocks_) {
		pHeader = reinterpret_cast<Header*>(buff_ + iBlockLast_ * (sizeof(Header) + bytesBlock_));
		iBlockLast_++;
	} else {
		::panic("no memory block is available in FixedPool(%d) with %d blocks", bytesBlock_, nBlocks_);
	}
	pHeader->ownerName = ownerName;
	return pHeader + 1;
}

void FixedPool::Free(void* p)
{
	Header* pHeader = reinterpret_cast<Header*>(p) - 1;
	pHeader->pHeaderNext = pHeaderFreedTop_;
	pHeaderFreedTop_ = pHeader;
}

bool FixedPool::IsUsed(const Header* pHeader) const
{
	return pHeader->p && (pHeader->p < buff_ || pHeader->p >= buff_ + GetBytesBuff());
}

void FixedPool::PrintUsage(Printable& printable) const
{
	const uint8_t* p = buff_;
	for (int iBlock = 0; iBlock < iBlockLast_; iBlock++, p += sizeof(Header) + bytesBlock_) {
		const Header* pHeader = reinterpret_cast<const Header*>(p);
		printable.Printf("%s", IsUsed(pHeader)? "*" : ".");
	}
	printable.Printf("\n");
}

int main()
{
	const char* ownerName = "test";
	FixedPool::Inst64.Initialize(8);
	::stdio_init_all();
	void* p;
	FixedPool& pool = FixedPool::Inst64;
	p = pool.Allocate(ownerName, 32);
	pool.PrintUsage(Stdio::GetPrintable());
	pool.Free(p);
	pool.PrintUsage(Stdio::GetPrintable());
	p = pool.Allocate(ownerName, 32);
	pool.PrintUsage(Stdio::GetPrintable());
	pool.Free(p);
	pool.PrintUsage(Stdio::GetPrintable());
	for (;;) ;
}
