//==============================================================================
// jxglib/Memory.h
//==============================================================================
#ifndef PICO_JXGLIB_MEMORY_H
#define PICO_JXGLIB_MEMORY_H
#include <malloc.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Memory
//------------------------------------------------------------------------------
class Memory {
protected:
	void* data_;
public:
	Memory(void* data) : data_{data} {}
	virtual ~Memory() {}
public:
	template<typename T> T* GetPointer() { return reinterpret_cast<T*>(data_); }
	virtual bool IsWritable() const { return !!data_; }
	virtual void Free() {}
};

//------------------------------------------------------------------------------
// MemoryConst
//------------------------------------------------------------------------------
class MemoryConst : public Memory {
public:
	MemoryConst(const void* data) : Memory(const_cast<void*>(data)) {}
	~MemoryConst() { /* do nothing */ }
public:
	virtual bool IsWritable() const override { return false; }
	virtual void Free() override { /* do nothing */ }
};

//------------------------------------------------------------------------------
// MemoryHeap
//------------------------------------------------------------------------------
class MemoryHeap : public Memory {
public:
	MemoryHeap(void* data) : Memory(data) {}
	~MemoryHeap() { ::free(data_); }
public:
	virtual void Free() override { ::free(data_); data_ = nullptr; }
};

}

#endif
