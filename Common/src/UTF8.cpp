//==============================================================================
// UTF8.cpp
//==============================================================================
#include "jxglib/UTF8.h"

namespace jxglib {

//------------------------------------------------------------------------------
// UTF8
//------------------------------------------------------------------------------
uint32_t UTF8::ToUTF32(const char* str, int* pBytes)
{
	uint32_t codeUTF32 = 0;
	Decoder decoder;
	const char* p = str;
	for ( ; *p; p++) {
		if (decoder.FeedChar(*p, &codeUTF32)) break;
	}
	if (pBytes) *pBytes = p - str + 1;
	return codeUTF32;
}

int UTF8::CountChars(const char* str)
{
	int nChars = 0;
	for (auto p = reinterpret_cast<const uint8_t*>(str); *p; p++) {
		if ((*p & 0xc0) != 0x80) nChars++; // Check if it's not a continuation byte
	}
	return nChars;
}

int UTF8::CountChars(const char* str, const char* strEnd)
{
	int nChars = 0;
	for (auto p = reinterpret_cast<const uint8_t*>(str); *p && p != reinterpret_cast<const uint8_t*>(strEnd); p++) {
		if ((*p & 0xc0) != 0x80) nChars++; // Check if it's not a continuation byte
	}
	return nChars;
}

//------------------------------------------------------------------------------
// UTF8::Decoder
//------------------------------------------------------------------------------
bool UTF8::Decoder::FeedChar(char ch, uint32_t* pCodeUTF32)
{
	strOrg_[iStrOrg_++] = ch;
	uint8_t chCasted = static_cast<uint8_t>(ch);
	if ((chCasted & 0x80) == 0x00) {
		*pCodeUTF32 = chCasted;
		strOrg_[iStrOrg_] = '\0';
		iStrOrg_ = 0;
		return true;
	} else if (nFollowers_ > 0) {
		if ((chCasted & 0xc0) == 0x80) {
			codeUTF32_ = (codeUTF32_ << 6) | (chCasted & 0x3f);
		} else {
			codeUTF32_ <<= 6;
		}
		nFollowers_--;
		if (nFollowers_ == 0) {
			*pCodeUTF32 = codeUTF32_;
			strOrg_[iStrOrg_] = '\0';
			iStrOrg_ = 0;
			return true;
		}
	} else if ((chCasted & 0xe0) == 0xc0) {
		codeUTF32_ = chCasted & 0x1f;
		nFollowers_ = 1;
	} else if ((chCasted & 0xf0) == 0xe0) {
		codeUTF32_ = chCasted & 0x0f;
		nFollowers_ = 2;
	} else if ((chCasted & 0xf8) == 0xf0) {
		codeUTF32_ = chCasted & 0x07;
		nFollowers_ = 3;
	} else if ((chCasted & 0xfc) == 0xf8) {
		codeUTF32_ = chCasted & 0x03;
		nFollowers_ = 4;
	} else {
		codeUTF32_ = chCasted & 0x01;
		nFollowers_ = 5;
	}
	return false;
}

}
