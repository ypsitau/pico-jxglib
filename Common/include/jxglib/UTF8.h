//==============================================================================
// jxglib/UTF8.h
//==============================================================================
#ifndef PICO_JXGLIB_UTF8_H
#define PICO_JXGLIB_UTF8_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// UTF8
//------------------------------------------------------------------------------
class UTF8 {
public:
	class Decoder {
	private:
		int nFollowers_;
		uint32_t codeUTF32_;
		char strOrg_[8];
		int iStrOrg_;
	public:
		Decoder() : nFollowers_{0}, codeUTF32_{0}, iStrOrg_{0} { strOrg_[0] = '\0'; }
		bool FeedChar(char ch, uint32_t* pCodeUTF32);
		const char* GetStringOrg() const { return strOrg_; }
	};
public:
	static uint32_t ToUTF32(const char* str, int* pBytes = nullptr);
	static int CountChars(const char* str);
	static int CountChars(const char* str, const char* strEnd);
};

}

#endif
