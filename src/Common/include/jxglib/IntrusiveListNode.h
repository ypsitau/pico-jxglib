//==============================================================================
// jxglib/IntrusiveListNode.h
//==============================================================================
#ifndef PICO_JXGLIB_INTRUSIVELISTNODE_H
#define PICO_JXGLIB_INTRUSIVELISTNODE_H
#include "pico/stdlib.h"
#include <stdio.h>

namespace jxglib {

//------------------------------------------------------------------------------
// IntrusiveListNode
//------------------------------------------------------------------------------
template<typename T> class IntrusiveListNode {
private:
	static IntrusiveListNode* pNodeHead_;
private:
	IntrusiveListNode* pNodeNext_;
public:
	IntrusiveListNode(bool addToListFlag = true) : pNodeNext_{nullptr} {
		if (!addToListFlag) {
			// nothing to do
		} else if (pNodeHead_) {
			IntrusiveListNode* pNode = pNodeHead_;
			for ( ; pNode->pNodeNext_; pNode = pNode->pNodeNext_) ;
			pNode->pNodeNext_ = this;
		} else {
			pNodeHead_ = this;
		}
	}
	virtual ~IntrusiveListNode() {
		if (pNodeHead_ == this) {
			pNodeHead_ = pNodeNext_;
		} else {
			IntrusiveListNode* pNode = pNodeHead_;
			for ( ; pNode; pNode = pNode->pNodeNext_) {
				if (pNode->pNodeNext_ == this) {
					pNode->pNodeNext_ = pNodeNext_;
					break;
				}
			}
		}
	}
public:
	static T* GetListNodeHead() { return reinterpret_cast<T*>(pNodeHead_); }
	T* GetListNodeNext() { return reinterpret_cast<T*>(pNodeNext_); }
	const T* GetListNodeNext() const { return reinterpret_cast<const T*>(pNodeNext_); }
public:
	static int CountListNodes() {
		int count = 0;
		for (IntrusiveListNode* pNode = pNodeHead_; pNode; pNode = pNode->pNodeNext_) count++;
		return count;
	}
};

template<typename T> inline IntrusiveListNode<T>* IntrusiveListNode<T>::pNodeHead_ = nullptr;

}

#endif
