//==============================================================================
// Dict.cpp
//==============================================================================
#include <stdio.h>
#include <memory.h>
#include "jxglib/Dict.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Dict
//------------------------------------------------------------------------------
const Dict Dict::None;

Dict::Dict()
{
}

void Dict::Add(Entry* pEntry)
{
	if (pDictEntry_) {
		pDictEntry_->Add(pEntry);
	} else {
		pDictEntry_.reset(pEntry);
	}
}

Dict::Entry* Dict::SetValue(const char* key, const char* value)
{
	if (pDictEntry_) {
		return pDictEntry_->SetValue(key, value);
	} else {
		pDictEntry_.reset(new Entry(key, value));
		return pDictEntry_.get();
	}
}

//------------------------------------------------------------------------------
// Dict::Entry
//------------------------------------------------------------------------------
Dict::Entry::Entry(const char* key)
{
	int lenKey = ::strlen(key);
	key_.reset(new char[lenKey + 1]);
	if (!key_) ::panic("failed to allocate memory for Dict::Entry");
	::memcpy(key_.get(), key, lenKey + 1);
}

Dict::Entry::Entry(const char* key, const char* value)
{
	int lenKey = ::strlen(key), lenValue = ::strlen(value);
	key_.reset(new char[lenKey + 1]);
	value_.reset(new char[lenValue + 1]);
	if (!key_ || !value_) ::panic("failed to allocate memory for Dict::Entry");
	::memcpy(key_.get(), key, lenKey + 1);
	::memcpy(value_.get(), value, lenValue + 1);
}

Dict::Entry::Entry(const char* key, int lenKey, const char* value)
{
	int lenValue = ::strlen(value);
	key_.reset(new char[lenKey + 1]);
	value_.reset(new char[lenValue + 1]);
	if (!key_ || !value_) ::panic("failed to allocate memory for Dict::Entry");
	::memcpy(key_.get(), key, lenKey);
	key_[lenKey] = '\0'; // Ensure null-termination
	::memcpy(value_.get(), value, lenValue + 1);
}

Dict::Entry* Dict::Entry::GetLast()
{
	Dict::Entry* pDictEntry = this;
	for ( ; pDictEntry->GetNext(); pDictEntry = pDictEntry->GetNext()) ;
	return pDictEntry;
}

void Dict::Entry::Add(Dict::Entry* pDictEntry)
{
	GetLast()->SetNext(pDictEntry);
}

Dict::Entry* Dict::Entry::Find(const char* key)
{
	for (Dict::Entry* pDictEntry = this; pDictEntry; pDictEntry = pDictEntry->GetNext()) {
		if (::strcmp(pDictEntry->GetKey(), key) == 0) return pDictEntry;
	}
	return nullptr;
}

Dict::Entry* Dict::Entry::Find(const char* key, int len)
{
	for (Dict::Entry* pDictEntry = this; pDictEntry; pDictEntry = pDictEntry->GetNext()) {
		if (::strncmp(pDictEntry->GetKey(), key, len) == 0) return pDictEntry;
	}
	return nullptr;
}

const char* Dict::Entry::Lookup(const char* key) const
{
	const Dict::Entry * pDictEntry = Find(key);
	return pDictEntry? pDictEntry->GetValue() : nullptr;
}

const char* Dict::Entry::Lookup(const char* key, int len) const
{
	const Dict::Entry * pDictEntry = Find(key, len);
	return pDictEntry? pDictEntry->GetValue() : nullptr;
}

void Dict::Entry::SetValue(const char* value)
{
	int lenValue = ::strlen(value);
	value_.reset(new char[lenValue + 1]);
	if (!value_) ::panic("failed to allocate memory for Dict::Entry");
	::memcpy(value_.get(), value, lenValue + 1);
}

Dict::Entry* Dict::Entry::SetValue(const char* key, const char* value)
{
	Dict::Entry* pDictEntry = Find(key);
	if (pDictEntry) {
		pDictEntry->SetValue(value);
	} else {
		pDictEntry = new Dict::Entry(key, value);
		Add(pDictEntry);
	}
	return pDictEntry;
}

}
