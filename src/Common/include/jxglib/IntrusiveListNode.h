//==============================================================================
// jxglib/IntrusiveListNode.h
//==============================================================================
#ifndef PICO_JXGLIB_INTRUSIVELISTNODE_H
#define PICO_JXGLIB_INTRUSIVELISTNODE_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// IntrusiveListNode
//------------------------------------------------------------------------------
template<typename T> class IntrusiveListNode {
private:
	static IntrusiveListNode* pHead_;
private:
	IntrusiveListNode* pNext_;
public:
	IntrusiveListNode(bool addToLinkFlag = true) : pNext_{nullptr} {
		if (!addToLinkFlag) {
			// nothing to do
		} else if (pHead_) {
			IntrusiveListNode* pLinkable = pHead_;
			for ( ; pLinkable->pNext_; pLinkable = pLinkable->pNext_) ;
			pLinkable->pNext_ = this;
		} else {
			pHead_ = this;
		}
	}
	~IntrusiveListNode() {
		if (pHead_ == this) {
			pHead_ = pNext_;
		} else {
			IntrusiveListNode* pLinkable = pHead_;
			for ( ; pLinkable; pLinkable = pLinkable->pNext_) {
				if (pLinkable->pNext_ == this) {
					pLinkable->pNext_ = pNext_;
					break;
				}
			}
		}
	}
public:
	static T* GetListNodeHead() { return reinterpret_cast<T*>(pHead_); }
	T* GetListNodeNext() { return reinterpret_cast<T*>(pNext_); }
	const T* GetListNodeNext() const { return reinterpret_cast<const T*>(pNext_); }
public:
	static int CountListNodes() {
		int count = 0;
		for (IntrusiveListNode* pNode = pHead_; pNode; pNode = pNode->pNext_) count++;
		return count;
	}
};

template<typename T> inline IntrusiveListNode<T>* IntrusiveListNode<T>::pHead_ = nullptr;

}

#endif
