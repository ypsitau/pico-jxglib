//==============================================================================
// jxglib/Referable.h
//==============================================================================
#ifndef PICO_JXGLIB_REFERABLE_H
#define PICO_JXGLIB_REFERABLE_H
#include "pico/stdlib.h"

#define DeclareReferable(T) \
T* Reference() const { \
	T* pCasted = const_cast<T*>(this); \
	pCasted->cntRef_++; \
	return pCasted; \
} \
static void Delete(T* p) { \
	if (!p) { \
	} else if (p->cntRef_ > 0) { \
		p->cntRef_--; \
	} else { \
		delete p; \
	} \
}

namespace jxglib {

//------------------------------------------------------------------------------
// RefPtr
//------------------------------------------------------------------------------
template<typename T> class RefPtr {
private:
	T* p_;
public:
	// Default constructor
	RefPtr(T* p = nullptr) noexcept : p_(p) {}
	// Copy constructor/operator
	RefPtr(const RefPtr& obj) = delete;
	RefPtr& operator=(const RefPtr& obj) = delete;
	// Move constructor/operator
	RefPtr(RefPtr&& obj) noexcept : p_(obj.p_) { obj.p_ = nullptr; }
	RefPtr& operator=(RefPtr&& obj) noexcept {
		T::Delete(p_);
		p_ = obj.p_;
		obj.p_ = nullptr;
		return *this;
	}
	// Destructor
	~RefPtr() { T::Delete(p_); }
public:
	T& operator*() { return *p_; }
	T& operator*() const { return *p_; }
	T* operator->() { return p_; }
	T* operator->() const { return p_; }
	void reset(T* p = nullptr) { T::Delete(p_); p_ = p; }
	T* get() { return p_; }
	T* get() const { return p_; }
	T* release() { T* p = p_; p_ = nullptr; return p; }
	T* Reference() const { return p_? p_->Reference() : nullptr; }
	T* Reference(size_t nRefs) const { return p_? p_->Reference(nRefs) : nullptr; }
	bool IsNull() const { return p_ == nullptr; }
	explicit operator bool() const { return static_cast<bool>(p_); }
};

//------------------------------------------------------------------------------
// Referable
//------------------------------------------------------------------------------
class Referable {
protected:
	int cntRef_;
public:
	Referable() : cntRef_(0) {}
protected:
	virtual ~Referable() {}
};

}

#endif
