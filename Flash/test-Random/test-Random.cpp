#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "jxglib/Stdio.h"
#include "jxglib/Printable.h"
#include "jxglib/Flash.h"

using namespace jxglib;

const uint32_t bytesXIP = 256;
uint8_t regionXIP[bytesXIP];
uint8_t buffToWrite[bytesXIP];
uint8_t buffToRead[bytesXIP];

class FlashDummy : public Flash {
private:
	uint8_t regionXIP_[bytesXIP];
public:
	FlashDummy() : Flash(16) { ::memset(regionXIP_, 0xff, sizeof(regionXIP_)); }
public:
	bool CompareRegion(const void* regionXIP) { return ::memcmp(regionXIP_, regionXIP, sizeof(regionXIP_)) == 0; }
	void DumpXIP() const { Dump(regionXIP_, sizeof(regionXIP_)); }
public:
	void Read(uint32_t offsetXIP, void* buff, uint32_t bytes) { Read_(offsetXIP, buff, bytes); }
	void Write(uint32_t offsetXIP, const void* buff, uint32_t bytes) { Write_(offsetXIP, buff, bytes); }
	void Sync() { Sync_(); }
public:
	virtual void Erase(uint32_t offsetXIP, uint32_t bytes) override;
	virtual void Program(uint32_t offsetXIP, const void* data, uint32_t bytes) override;
	virtual void CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes) override;
};

uint32_t GetRand(uint32_t n)
{
	return static_cast<uint32_t>((static_cast<uint64_t>(::get_rand_32()) * n) >> 32);
}

FlashDummy flash;

int main()
{
	::memset(regionXIP, 0xff, sizeof(regionXIP));
	for (int i = 0; i < sizeof(buffToWrite); i++) buffToWrite[i] = static_cast<uint8_t>(i);
	::stdio_init_all();
	for (int iTrial = 1; ; iTrial++) {
		if (iTrial % 10000 == 0) ::printf("# %d\n", iTrial);
		do {
			uint32_t offsetXIP = GetRand(bytesXIP - 16);
			uint32_t bytes = GetRand(bytesXIP - offsetXIP);
			flash.Write(offsetXIP, buffToWrite, bytes);
			::memcpy(regionXIP + offsetXIP, buffToWrite, bytes);
		} while (0);
		do {
			uint32_t offsetXIP = GetRand(bytesXIP - 16);
			uint32_t bytes = GetRand(bytesXIP - offsetXIP);
			flash.Read(offsetXIP, buffToRead, bytes);
			if (::memcmp(regionXIP + offsetXIP, buffToRead, bytes) != 0) {
				::printf("Read Error\n");
			}
		} while (0);
		if (GetRand(100) == 0) {
			flash.Sync();
			if (!flash.CompareRegion(regionXIP)) ::printf("Verify Error\n");
		}
	}
}

//------------------------------------------------------------------------------
// FlashDummy
//------------------------------------------------------------------------------
void FlashDummy::Erase(uint32_t offsetXIP, uint32_t bytes)
{
	::memset(regionXIP_ + offsetXIP, 0xff, bytes);
}

void FlashDummy::Program(uint32_t offsetXIP, const void* data, uint32_t bytes)
{
	::memcpy(regionXIP_ + offsetXIP, data, bytes);
}

void FlashDummy::CopyMemory(void* dst, uint32_t offsetDst, const void* src, uint32_t offsetSrc, uint32_t bytes)
{
	void* pDst = (dst == reinterpret_cast<void*>(XIP_BASE))? regionXIP_ : dst;
	const void *pSrc = (src == reinterpret_cast<const void*>(XIP_BASE))? regionXIP_ : src;
	::memcpy(reinterpret_cast<uint8_t*>(pDst) + offsetDst, reinterpret_cast<const uint8_t*>(pSrc) + offsetSrc, bytes);
}
