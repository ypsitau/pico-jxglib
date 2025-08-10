//==============================================================================
// jxglib/Resets.h
//==============================================================================
#ifndef PICO_JXGLIB_RESETS_H
#define PICO_JXGLIB_RESETS_H
#include "pico/stdlib.h"
#include "hardware/resets.h"
#include "jxglib/Common.h"

namespace jxglib {

inline void ResetMask(uint mask) { ::reset_block_mask(mask); }
inline void UnresetMask(uint mask) { ::unreset_block_mask(mask); }
inline void UnresetBlockingMask(uint mask) { ::unreset_block_mask_wait_blocking(mask); }
inline void ResetUnresetBlockingMask(uint mask) {
	::reset_block_mask(mask); ::unreset_block_mask_wait_blocking(mask);
}

template<typename... Args> void Reset(Args... bits)
{
	static_assert(sizeof...(bits) > 0, "At least one bit must be specified");
	ResetMask(((1u << bits) | ...));
}

template<typename... Args> void Unreset(Args... bits)
{
	static_assert(sizeof...(bits) > 0, "At least one bit must be specified");
	UnresetMask(((1u << bits) | ...));
}

template<typename... Args> void UnresetBlocking(Args... bits)
{
	static_assert(sizeof...(bits) > 0, "At least one bit must be specified");
	UnresetBlockingMask(((1u << bits) | ...));
}

template<typename... Args> void ResetUnresetBlocking(Args... bits)
{
	static_assert(sizeof...(bits) > 0, "At least one bit must be specified");
	ResetUnresetBlocking(((1u << bits) | ...));
}

}

#endif
