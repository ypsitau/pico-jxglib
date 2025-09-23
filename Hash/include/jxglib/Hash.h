//==============================================================================
// jxglib/Hash.h
//==============================================================================
#ifndef PICO_JXGLIB_HASH_H
#define PICO_JXGLIB_HASH_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib::Hash {

//------------------------------------------------------------------------------
// SHA1
//------------------------------------------------------------------------------
class SHA1 {
private:
	uint32_t state_[5];    // Hash state (A, B, C, D, E)
	uint32_t count_[2];    // Message length in bits (lo, hi)
	uint8_t buffer_[64];   // Input buffer
	uint8_t digest_[20];   // Message digest
	bool finalized_;       // Whether digest is finalized
	char hexString_[41];   // Hex string representation
private:
	static const uint32_t K[4];
private:
	void transform_(const uint8_t block[64]);
	uint32_t rotateLeft_(uint32_t value, uint32_t amount);
public:
	SHA1();
	void Reset();
	void Update(const void* data, size_t size);
	const char* Finish();
};

}

#endif
