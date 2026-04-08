//==============================================================================
// Hash.cpp
//==============================================================================
#include "jxglib/Hash.h"
#include <cstring>

namespace jxglib::Hash {

//------------------------------------------------------------------------------
// Hash::MD5 - MD5 implementation
//------------------------------------------------------------------------------
// MD5 constants (sine table values)
const uint32_t MD5::T[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

MD5::MD5()
{
	Reset();
}

void MD5::Reset()
{
	// Initialize MD5 hash values
	state_[0] = 0x67452301;
	state_[1] = 0xefcdab89;
	state_[2] = 0x98badcfe;
	state_[3] = 0x10325476;
	
	count_ = 0;
	std::memset(buffer_, 0, 64);
	std::memset(digest_, 0, 16);
	finalized_ = false;
}

uint32_t MD5::rotateLeft_(uint32_t value, uint32_t amount)
{
	return (value << amount) | (value >> (32 - amount));
}

uint32_t MD5::F_(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) | (~x & z);
}

uint32_t MD5::G_(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & z) | (y & ~z);
}

uint32_t MD5::H_(uint32_t x, uint32_t y, uint32_t z)
{
	return x ^ y ^ z;
}

uint32_t MD5::I_(uint32_t x, uint32_t y, uint32_t z)
{
	return y ^ (x | ~z);
}

void MD5::transform_(const uint8_t block[64])
{
	uint32_t X[16];
	uint32_t A = state_[0];
	uint32_t B = state_[1];
	uint32_t C = state_[2];
	uint32_t D = state_[3];
	
	// Convert block to little-endian 32-bit words
	for (int i = 0; i < 16; i++) {
		X[i] = block[i * 4] | (block[i * 4 + 1] << 8) |
		       (block[i * 4 + 2] << 16) | (block[i * 4 + 3] << 24);
	}
	
	// Round 1
	static const int s1[4] = {7, 12, 17, 22};
	for (int i = 0; i < 16; i++) {
		uint32_t temp = A + F_(B, C, D) + X[i] + T[i];
		temp = rotateLeft_(temp, s1[i % 4]) + B;
		A = D; D = C; C = B; B = temp;
	}
	
	// Round 2
	static const int s2[4] = {5, 9, 14, 20};
	static const int k2[16] = {1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12};
	for (int i = 0; i < 16; i++) {
		uint32_t temp = A + G_(B, C, D) + X[k2[i]] + T[16 + i];
		temp = rotateLeft_(temp, s2[i % 4]) + B;
		A = D; D = C; C = B; B = temp;
	}
	
	// Round 3
	static const int s3[4] = {4, 11, 16, 23};
	static const int k3[16] = {5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2};
	for (int i = 0; i < 16; i++) {
		uint32_t temp = A + H_(B, C, D) + X[k3[i]] + T[32 + i];
		temp = rotateLeft_(temp, s3[i % 4]) + B;
		A = D; D = C; C = B; B = temp;
	}
	
	// Round 4
	static const int s4[4] = {6, 10, 15, 21};
	static const int k4[16] = {0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9};
	for (int i = 0; i < 16; i++) {
		uint32_t temp = A + I_(B, C, D) + X[k4[i]] + T[48 + i];
		temp = rotateLeft_(temp, s4[i % 4]) + B;
		A = D; D = C; C = B; B = temp;
	}
	
	// Add this chunk's hash to result so far
	state_[0] += A;
	state_[1] += B;
	state_[2] += C;
	state_[3] += D;
}

void MD5::Put(const void* data, size_t size)
{
	if (finalized_) return;
	
	const uint8_t* input = static_cast<const uint8_t*>(data);
	uint64_t index = (count_ >> 3) & 0x3F;
	
	// Update number of bits
	count_ += (uint64_t)(size << 3);
	
	uint64_t partLen = 64 - index;
	
	// Transform as many times as possible
	uint64_t i = 0;
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

const char* MD5::Complete()
{
	if (finalized_) return hexString_;
	
	// Prepare padding
	uint8_t bits[8];
	for (int i = 0; i < 8; i++) {
		bits[i] = (count_ >> (i * 8)) & 0xFF;  // Little-endian
	}
	
	// Pad message to 448 bits mod 512
	uint64_t index = (count_ >> 3) & 0x3F;
	uint64_t padLen = (index < 56) ? (56 - index) : (120 - index);
	
	uint8_t padding[64];
	padding[0] = 0x80;
	std::memset(padding + 1, 0, padLen - 1);
	
	Put(padding, padLen);
	Put(bits, 8);
	
	// Store digest in byte array (little-endian)
	for (int i = 0; i < 4; i++) {
		digest_[i * 4] = state_[i] & 0xFF;
		digest_[i * 4 + 1] = (state_[i] >> 8) & 0xFF;
		digest_[i * 4 + 2] = (state_[i] >> 16) & 0xFF;
		digest_[i * 4 + 3] = (state_[i] >> 24) & 0xFF;
	}
	
	// Convert to hex string
	static const char hexChars[] = "0123456789abcdef";
	for (int i = 0; i < 16; i++) {
		hexString_[i * 2] = hexChars[digest_[i] >> 4];
		hexString_[i * 2 + 1] = hexChars[digest_[i] & 0x0F];
	}
	hexString_[32] = '\0';
	
	finalized_ = true;
	return hexString_;
}

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

void SHA1::Put(const void* data, size_t size)
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

const char* SHA1::Complete()
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
	
	Put(padding, padLen);
	Put(bits, 8);
	
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

//------------------------------------------------------------------------------
// Hash::SHA256 - SHA-256 implementation
//------------------------------------------------------------------------------
// SHA-256 constants
const uint32_t SHA256::K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

SHA256::SHA256()
{
	Reset();
}

void SHA256::Reset()
{
	// Initialize SHA-256 hash values
	state_[0] = 0x6a09e667;
	state_[1] = 0xbb67ae85;
	state_[2] = 0x3c6ef372;
	state_[3] = 0xa54ff53a;
	state_[4] = 0x510e527f;
	state_[5] = 0x9b05688c;
	state_[6] = 0x1f83d9ab;
	state_[7] = 0x5be0cd19;
	
	count_ = 0;
	std::memset(buffer_, 0, 64);
	std::memset(digest_, 0, 32);
	finalized_ = false;
}

uint32_t SHA256::rotateRight_(uint32_t value, uint32_t amount)
{
	return (value >> amount) | (value << (32 - amount));
}

uint32_t SHA256::choose_(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (~x & z);
}

uint32_t SHA256::majority_(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t SHA256::sigma0_(uint32_t x)
{
	return rotateRight_(x, 2) ^ rotateRight_(x, 13) ^ rotateRight_(x, 22);
}

uint32_t SHA256::sigma1_(uint32_t x)
{
	return rotateRight_(x, 6) ^ rotateRight_(x, 11) ^ rotateRight_(x, 25);
}

uint32_t SHA256::gamma0_(uint32_t x)
{
	return rotateRight_(x, 7) ^ rotateRight_(x, 18) ^ (x >> 3);
}

uint32_t SHA256::gamma1_(uint32_t x)
{
	return rotateRight_(x, 17) ^ rotateRight_(x, 19) ^ (x >> 10);
}

void SHA256::transform_(const uint8_t block[64])
{
	uint32_t W[64];
	uint32_t A = state_[0];
	uint32_t B = state_[1];
	uint32_t C = state_[2];
	uint32_t D = state_[3];
	uint32_t E = state_[4];
	uint32_t F = state_[5];
	uint32_t G = state_[6];
	uint32_t H = state_[7];
	
	// Prepare the message schedule
	for (int i = 0; i < 16; i++) {
		W[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) |
		       (block[i * 4 + 2] << 8) | block[i * 4 + 3];
	}
	
	for (int i = 16; i < 64; i++) {
		W[i] = gamma1_(W[i - 2]) + W[i - 7] + gamma0_(W[i - 15]) + W[i - 16];
	}
	
	// Main loop
	for (int i = 0; i < 64; i++) {
		uint32_t temp1 = H + sigma1_(E) + choose_(E, F, G) + K[i] + W[i];
		uint32_t temp2 = sigma0_(A) + majority_(A, B, C);
		
		H = G;
		G = F;
		F = E;
		E = D + temp1;
		D = C;
		C = B;
		B = A;
		A = temp1 + temp2;
	}
	
	// Add this chunk's hash to result so far
	state_[0] += A;
	state_[1] += B;
	state_[2] += C;
	state_[3] += D;
	state_[4] += E;
	state_[5] += F;
	state_[6] += G;
	state_[7] += H;
}

void SHA256::Put(const void* data, size_t size)
{
	if (finalized_) return;
	
	const uint8_t* input = static_cast<const uint8_t*>(data);
	uint64_t index = (count_ >> 3) & 0x3F;
	
	// Update number of bits
	count_ += (uint64_t)(size << 3);
	
	uint64_t partLen = 64 - index;
	
	// Transform as many times as possible
	uint64_t i = 0;
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

const char* SHA256::Complete()
{
	if (finalized_) return hexString_;
	
	// Prepare padding
	uint8_t bits[8];
	for (int i = 0; i < 8; i++) {
		bits[i] = (count_ >> (56 - i * 8)) & 0xFF;
	}
	
	// Pad message to 448 bits mod 512
	uint64_t index = (count_ >> 3) & 0x3F;
	uint64_t padLen = (index < 56) ? (56 - index) : (120 - index);
	
	uint8_t padding[64];
	padding[0] = 0x80;
	std::memset(padding + 1, 0, padLen - 1);
	
	Put(padding, padLen);
	Put(bits, 8);
	
	// Store digest in byte array
	for (int i = 0; i < 8; i++) {
		digest_[i * 4] = (state_[i] >> 24) & 0xFF;
		digest_[i * 4 + 1] = (state_[i] >> 16) & 0xFF;
		digest_[i * 4 + 2] = (state_[i] >> 8) & 0xFF;
		digest_[i * 4 + 3] = state_[i] & 0xFF;
	}
	
	// Convert to hex string
	static const char hexChars[] = "0123456789abcdef";
	for (int i = 0; i < 32; i++) {
		hexString_[i * 2] = hexChars[digest_[i] >> 4];
		hexString_[i * 2 + 1] = hexChars[digest_[i] & 0x0F];
	}
	hexString_[64] = '\0';
	
	finalized_ = true;
	return hexString_;
}

}
