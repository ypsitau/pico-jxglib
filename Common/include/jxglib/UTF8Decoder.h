//==============================================================================
// jxglib/UTF8Decoder.h
//==============================================================================
#ifndef PICO_JXGLIB_UTF8DECODER_H
#define PICO_JXGLIB_UTF8DECODER_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// UTF8Decoder
//------------------------------------------------------------------------------
class UTF8Decoder {
private:
	int nFollowers_;
	uint32_t codeUTF32_;
	char strOrg_[8];
	int iStrOrg_;
public:
	UTF8Decoder() : nFollowers_{0}, codeUTF32_{0}, iStrOrg_{0} { strOrg_[0] = '\0'; }
	bool FeedChar(char ch, uint32_t* pCodeUTF32);
	const char* GetStringOrg() const { return strOrg_; }
	static uint32_t ToUTF32(const char* str, int* pBytes = nullptr);
};

}

#endif
