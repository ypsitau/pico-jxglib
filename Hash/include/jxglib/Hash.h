//==============================================================================
// jxglib/Hash.h
//==============================================================================
#ifndef PICO_JXGLIB_HASH_H
#define PICO_JXGLIB_HASH_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib::Hash {

//------------------------------------------------------------------------------
// MD5
//------------------------------------------------------------------------------
class MD5 {
private:
	uint32_t state_[4];    // Hash state (A, B, C, D)
	uint64_t count_;       // Message length in bits
	uint8_t buffer_[64];   // Input buffer
	uint8_t digest_[16];   // Message digest
	bool finalized_;       // Whether digest is finalized
	char hexString_[33];   // Hex string representation
private:
	static const uint32_t T[64];
private:
	void transform_(const uint8_t block[64]);
	uint32_t rotateLeft_(uint32_t value, uint32_t amount);
	uint32_t F_(uint32_t x, uint32_t y, uint32_t z);
	uint32_t G_(uint32_t x, uint32_t y, uint32_t z);
	uint32_t H_(uint32_t x, uint32_t y, uint32_t z);
	uint32_t I_(uint32_t x, uint32_t y, uint32_t z);
public:
	MD5();
	void Reset();
	void Update(const void* data, size_t size);
	const char* Finish();
};

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

//------------------------------------------------------------------------------
// SHA256 (SHA-2 family)
//------------------------------------------------------------------------------
class SHA256 {
private:
	uint32_t state_[8];    // Hash state (A, B, C, D, E, F, G, H)
	uint64_t count_;       // Message length in bits
	uint8_t buffer_[64];   // Input buffer
	uint8_t digest_[32];   // Message digest
	bool finalized_;       // Whether digest is finalized
	char hexString_[65];   // Hex string representation
private:
	static const uint32_t K[64];
private:
	void transform_(const uint8_t block[64]);
	uint32_t rotateRight_(uint32_t value, uint32_t amount);
	uint32_t choose_(uint32_t x, uint32_t y, uint32_t z);
	uint32_t majority_(uint32_t x, uint32_t y, uint32_t z);
	uint32_t sigma0_(uint32_t x);
	uint32_t sigma1_(uint32_t x);
	uint32_t gamma0_(uint32_t x);
	uint32_t gamma1_(uint32_t x);
public:
	SHA256();
	void Reset();
	void Update(const void* data, size_t size);
	const char* Finish();
};

}

#endif
