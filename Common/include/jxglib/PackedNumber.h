//==============================================================================
// jxglib/PackedNumber
//==============================================================================
#ifndef PICO_JXGLIB_PACKEDNUMBER_H
#define PICO_JXGLIB_PACKEDNUMBER_H
#include "pico/stdlib.h"

#define Packed_uint16(x) \
uint8_t x##_uint16_b0, x##_uint16_b8

#define Packed_uint16_BE(x) \
uint8_t x##_uint16_b8, x##_uint16_b0

#define Packed_uint32(x) \
uint8_t x##_uint32_b0, x##_uint32_b8, x##_uint32_b16, x##_uint32_b24

#define Packed_uint32_BE(x) \
uint8_t x##_uint32_b24, x##_uint32_b16, x##_uint32_b8, x##_uint32_b0

#define Packed_uint64(x) \
uint8_t x##_uint64_b0, x##_uint64_b8, x##_uint64_b16, x##_uint64_b24, \
	x##_uint64_b32, x##_uint64_b40, x##_uint64_b48, x##_uint64_b56

#define Packed_uint64_BE(x) \
uint8_t x##_uint64_b56, x##_uint64_b48, x##_uint64_b40, x##_uint64_b32, \
	x##_uint64_b24, x##_uint64_b16, x##_uint64_b8, x##_uint64_b0

#define Packed_int16(x) Packed_uint16(x)
#define Packed_int16_BE(x) Packed_uint16_BE(x)

#define Packed_int32(x) Packed_uint32(x)
#define Packed_int32_BE(x) Packed_uint32_BE(x)

#define Packed_int64(x) Packed_uint64(x)
#define Packed_int64_BE(x) Packed_uint64_BE(x)

#define Packed_float(x) Packed_uint32(x)
#define Packed_double(x) Packed_uint64(x)

#define Pack_uint16(x, n) do { \
	uint16_t casted = static_cast<uint16_t>(n); \
	x##_uint16_b0 = static_cast<uint8_t>((casted >> 0) & 0xff); \
	x##_uint16_b8 = static_cast<uint8_t>((casted >> 8) & 0xff); \
} while (0)

#define Pack_uint32(x, n) do { \
	uint32_t casted = static_cast<uint32_t>(n); \
	x##_uint32_b0 = static_cast<uint8_t> ((casted >>  0) & 0xff); \
	x##_uint32_b8 = static_cast<uint8_t> ((casted >>  8) & 0xff); \
	x##_uint32_b16 = static_cast<uint8_t>((casted >> 16) & 0xff); \
	x##_uint32_b24 = static_cast<uint8_t>((casted >> 24) & 0xff); \
} while (0)

#define Pack_uint64(x, n) do { \
	uint64_t casted = static_cast<uint64_t>(n); \
	x##_uint64_b0 = static_cast<uint8_t> ((casted >>  0) & 0xff); \
	x##_uint64_b8 = static_cast<uint8_t> ((casted >>  8) & 0xff); \
	x##_uint64_b16 = static_cast<uint8_t>((casted >> 16) & 0xff); \
	x##_uint64_b24 = static_cast<uint8_t>((casted >> 24) & 0xff); \
	x##_uint64_b32 = static_cast<uint8_t>((casted >> 32) & 0xff); \
	x##_uint64_b40 = static_cast<uint8_t>((casted >> 40) & 0xff); \
	x##_uint64_b48 = static_cast<uint8_t>((casted >> 48) & 0xff); \
	x##_uint64_b56 = static_cast<uint8_t>((casted >> 56) & 0xff); \
} while (0)

#define Pack_float(x, n) do { \
	float buff = n; \
	uint32_t casted = *reinterpret_cast<const uint32_t*>(&buff); \
	x##_uint32_b0 = static_cast<uint8_t> ((casted >>  0) & 0xff); \
	x##_uint32_b8 = static_cast<uint8_t> ((casted >>  8) & 0xff); \
	x##_uint32_b16 = static_cast<uint8_t>((casted >> 16) & 0xff); \
	x##_uint32_b24 = static_cast<uint8_t>((casted >> 24) & 0xff); \
} while (0)

#define Pack_double(x, n) do { \
	double buff = n; \
	uint64_t casted = *reinterpret_cast<const uint64_t*>(&buff); \
	x##_uint64_b0 = static_cast<uint8_t> ((casted >>  0) & 0xff); \
	x##_uint64_b8 = static_cast<uint8_t> ((casted >>  8) & 0xff); \
	x##_uint64_b16 = static_cast<uint8_t>((casted >> 16) & 0xff); \
	x##_uint64_b24 = static_cast<uint8_t>((casted >> 24) & 0xff); \
	x##_uint64_b32 = static_cast<uint8_t>((casted >> 32) & 0xff); \
	x##_uint64_b40 = static_cast<uint8_t>((casted >> 40) & 0xff); \
	x##_uint64_b48 = static_cast<uint8_t>((casted >> 48) & 0xff); \
	x##_uint64_b56 = static_cast<uint8_t>((casted >> 56) & 0xff); \
} while (0)

#define Unpack_uint16(x) ( \
	(static_cast<uint16_t>(x##_uint16_b0) << 0) | \
	(static_cast<uint16_t>(x##_uint16_b8) << 8) \
)

#define Unpack_uint32(x) ( \
	(static_cast<uint32_t>(x##_uint32_b0)  <<  0) | \
	(static_cast<uint32_t>(x##_uint32_b8)  <<  8) | \
	(static_cast<uint32_t>(x##_uint32_b16) << 16) | \
	(static_cast<uint32_t>(x##_uint32_b24) << 24) \
)

#define Unpack_uint64(x) ( \
	(static_cast<uint64_t>(x##_uint64_b0)  <<  0) | \
	(static_cast<uint64_t>(x##_uint64_b8)  <<  8) | \
	(static_cast<uint64_t>(x##_uint64_b16) << 16) | \
	(static_cast<uint64_t>(x##_uint64_b24) << 24) | \
	(static_cast<uint64_t>(x##_uint64_b32) << 32) | \
	(static_cast<uint64_t>(x##_uint64_b40) << 40) | \
	(static_cast<uint64_t>(x##_uint64_b48) << 48) | \
	(static_cast<uint64_t>(x##_uint64_b56) << 56) \
)

#define Pack_int16(x, n) Pack_uint16(x, n)
#define Pack_int32(x, n) Pack_uint32(x, n)
#define Pack_int64(x, n) Pack_uint64(x, n)

#define Unpack_int16(x) static_cast<Int16>(Unpack_uint16(x))
#define Unpack_int32(x) static_cast<Int32>(Unpack_uint32(x))
#define Unpack_int64(x) static_cast<int64>(Unpack_uint64(x))

#endif
