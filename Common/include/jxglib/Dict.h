//==============================================================================
// jxglib/Entry.h
//==============================================================================
#ifndef PICO_JXGLIB_DICT_H
#define PICO_JXGLIB_DICT_H
#include <memory>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------------
class Dict {
public:
	class Entry {
	private:
		std::unique_ptr<char[]> key_;
		std::unique_ptr<char[]> value_;
		std::unique_ptr<Entry> pNext_;
	public:
		Entry();
		Entry(const char* key);
		Entry(const char* key, const char* value);
		Entry(const char* key, int lenKey, const char* value);
	public:
		void SetNext(Entry* pNext) { pNext_.reset(pNext); }
		Entry* GetNext() { return pNext_.get(); }
		const Entry* GetNext() const { return pNext_.get(); }
		Entry* GetLast();
		void Add(Entry* pDictEntry);
		Entry* Find(const char* key);
		Entry* Find(const char* key, int len);
		const Entry* Find(const char* key) const { return const_cast<Entry*>(this)->Find(key); }
		const Entry* Find(const char* key, int len) const { return const_cast<Entry*>(this)->Find(key, len); }
		const char* Lookup(const char* key) const;
		const char* Lookup(const char* key, int len) const;
		void SetValue(const char* value);
		Entry* SetValue(const char* key, const char* value);
		Entry* SetValue(const char* key, int len, const char* value);
	public:
		bool HasValue() const { return !!value_; }
		const char* GetKey() const { return key_? key_.get() : ""; }
		const char* GetKeyRaw() const { return key_.get(); }
		const char* GetValue() const { return value_? value_.get() : ""; }
		const char* GetValueRaw() const { return value_.get(); }
	};
private:
	std::unique_ptr<Entry> pDictEntry_;
public:
	static const Dict None;
public:
	Dict();
public:
	void Add(Entry* pEntry);
	Entry* GetFirst() { return pDictEntry_.get(); }
	const Entry* GetFirst() const { return pDictEntry_.get(); }
	Entry* Find(const char* key) { return pDictEntry_? pDictEntry_->Find(key) : nullptr; }
	Entry* Find(const char* key, int len) { return pDictEntry_? pDictEntry_->Find(key, len) : nullptr; }
	const Entry* Find(const char* key) const { return pDictEntry_? pDictEntry_->Find(key) : nullptr; }
	const Entry* Find(const char* key, int len) const { return pDictEntry_? pDictEntry_->Find(key, len) : nullptr; }
	const char* Lookup(const char* key) const { return pDictEntry_? pDictEntry_->Lookup(key) : nullptr; }
	const char* Lookup(const char* key, int len) const { return pDictEntry_? pDictEntry_->Lookup(key, len) : nullptr; }
	Entry* SetValue(const char* key, const char* value);
	Entry* SetValue(const char* key, int len, const char* value);
};

}

#endif
