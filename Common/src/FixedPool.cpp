//==============================================================================
// FixedPool.cpp
//==============================================================================
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "jxglib/FixedPool.h"

namespace jxglib {

FixedPool FixedPool::InstSmall("FixedPool::InstSmall", 32);
FixedPool FixedPool::InstMedium("FixedPool::InstMedium", 64);
FixedPool FixedPool::InstLarge("FixedPool::InstLarge", 128);

FixedPool::FixedPool(const char* name, int bytesBlock) :
    name_{name}, buff_{nullptr}, bytesBlock_{bytesBlock}, nBlocks_{0}, pHeaderFreedTop_{nullptr}
{
}

void FixedPool::Initialize(int nBlocks)
{
    nBlocks_ = nBlocks;
    int bytesBuff = GetBytesBuff();
    buff_ = reinterpret_cast<uint8_t*>(::malloc(bytesBuff));
    if (!buff_) ::panic("failed to allocate memory in %s.Initialize()", GetName());
    ::memset(buff_, 0x00, bytesBuff);
}

void* FixedPool::Allocate(const char* ownerName, int bytes)
{
    Header* pHeader;
    if (!buff_) {
        ::panic("%s is not initialized", GetName());
    } else if (bytes > bytesBlock_) {
        ::panic("the size of %s is %d bytes, which exceeds %d bytes of %s", ownerName, bytes, bytesBlock_, GetName());
    } else if (pHeaderFreedTop_) {
        pHeader = pHeaderFreedTop_;
        pHeaderFreedTop_ = pHeaderFreedTop_->pHeaderNext;
    } else if (iBlockLast_ < nBlocks_) {
        pHeader = reinterpret_cast<Header*>(buff_ + iBlockLast_ * (sizeof(Header) + bytesBlock_));
        iBlockLast_++;
    } else {
        ::panic("no memory block is available in %s with %d blocks", GetName(), nBlocks_);
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

void FixedPool::PrintUsage() const
{
    const uint8_t* p = buff_;
    for (int iBlock = 0; iBlock < iBlockLast_; iBlock++, p += sizeof(Header) + bytesBlock_) {
        const Header* pHeader = reinterpret_cast<const Header*>(p);
        ::printf("%c", IsUsed(pHeader)? *pHeader->ownerName : '.');
    }
    ::printf("\n");
}
    
}

