//==============================================================================
// Hash.cpp
//==============================================================================
#include "jxglib/Hash.h"
#include <cstring>

namespace jxglib::Hash {

//------------------------------------------------------------------------------
// Hash::SHA1 - SHA-1 implementation
//------------------------------------------------------------------------------
// SHA-1 constants
const uint32_t SHA1::K[4] = {
	0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
};

SHA1::SHA1()
{
	Reset();
}

void SHA1::Reset()
{
	// Initialize SHA-1 hash values
	state_[0] = 0x67452301;
	state_[1] = 0xEFCDAB89;
	state_[2] = 0x98BADCFE;
	state_[3] = 0x10325476;
	state_[4] = 0xC3D2E1F0;
	
	count_[0] = 0;
	count_[1] = 0;
	
	std::memset(buffer_, 0, 64);
	std::memset(digest_, 0, 20);
	finalized_ = false;
}

uint32_t SHA1::rotateLeft_(uint32_t value, uint32_t amount)
{
	return (value << amount) | (value >> (32 - amount));
}

void SHA1::transform_(const uint8_t block[64])
{
	uint32_t W[80];
	uint32_t A = state_[0];
	uint32_t B = state_[1];
	uint32_t C = state_[2];
	uint32_t D = state_[3];
	uint32_t E = state_[4];
	
	// Prepare the message schedule
	for (int i = 0; i < 16; i++) {
		W[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) |
		       (block[i * 4 + 2] << 8) | block[i * 4 + 3];
	}
	
	for (int i = 16; i < 80; i++) {
		W[i] = rotateLeft_(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);
	}
	
	// Main loop
	for (int i = 0; i < 80; i++) {
		uint32_t f, k;
		
		if (i < 20) {
			f = (B & C) | ((~B) & D);
			k = K[0];
		} else if (i < 40) {
			f = B ^ C ^ D;
			k = K[1];
		} else if (i < 60) {
			f = (B & C) | (B & D) | (C & D);
			k = K[2];
		} else {
			f = B ^ C ^ D;
			k = K[3];
		}
		
		uint32_t temp = rotateLeft_(A, 5) + f + E + k + W[i];
		E = D;
		D = C;
		C = rotateLeft_(B, 30);
		B = A;
		A = temp;
	}
	
	// Add this chunk's hash to result so far
	state_[0] += A;
	state_[1] += B;
	state_[2] += C;
	state_[3] += D;
	state_[4] += E;
}

void SHA1::Update(const void* data, size_t size)
{
	if (finalized_) return;
	
	const uint8_t* input = static_cast<const uint8_t*>(data);
	uint32_t index = (count_[0] >> 3) & 0x3F;
	
	// Update number of bits
	if ((count_[0] += (uint32_t)(size << 3)) < (size << 3)) {
		count_[1]++;
	}
	count_[1] += (uint32_t)(size >> 29);
	
	uint32_t partLen = 64 - index;
	
	// Transform as many times as possible
	uint32_t i = 0;
	if (size >= partLen) {
		std::memcpy(&buffer_[index], input, partLen);
		transform_(buffer_);
		
		for (i = partLen; i + 63 < size; i += 64) {
			transform_(&input[i]);
		}
		
		index = 0;
	}
	
	// Buffer remaining input
	std::memcpy(&buffer_[index], &input[i], size - i);
}

const char* SHA1::Finish()
{
	if (finalized_) return hexString_;
	
	// Prepare padding
	uint8_t bits[8];
	for (int i = 0; i < 4; i++) {
		bits[i] = (count_[1] >> (24 - i * 8)) & 0xFF;
		bits[i + 4] = (count_[0] >> (24 - i * 8)) & 0xFF;
	}
	
	// Pad message to 448 bits mod 512
	uint32_t index = (count_[0] >> 3) & 0x3F;
	uint32_t padLen = (index < 56) ? (56 - index) : (120 - index);
	
	uint8_t padding[64];
	padding[0] = 0x80;
	std::memset(padding + 1, 0, padLen - 1);
	
	Update(padding, padLen);
	Update(bits, 8);
	
	// Store digest in byte array
	for (int i = 0; i < 5; i++) {
		digest_[i * 4] = (state_[i] >> 24) & 0xFF;
		digest_[i * 4 + 1] = (state_[i] >> 16) & 0xFF;
		digest_[i * 4 + 2] = (state_[i] >> 8) & 0xFF;
		digest_[i * 4 + 3] = state_[i] & 0xFF;
	}
	
	// Convert to hex string
	static const char hexChars[] = "0123456789abcdef";
	for (int i = 0; i < 20; i++) {
		hexString_[i * 2] = hexChars[digest_[i] >> 4];
		hexString_[i * 2 + 1] = hexChars[digest_[i] & 0x0F];
	}
	hexString_[40] = '\0';
	
	finalized_ = true;
	return hexString_;
}

}
