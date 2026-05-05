//==============================================================================
// UF2Installer
//==============================================================================
#include <stdlib.h>
#include "boot/uf2.h"
#include "jxglib/USBDevice/MSCDrive.h"

namespace jxglib {

//-----------------------------------------------------------------------------
// UF2Installer
//-----------------------------------------------------------------------------
class UF2Installer : public USBDevice::MSCDrive::HookHandler {
private:
	Printable& terr_;
	bool completeFlag_;
	uint32_t flashBytes_;
	uint32_t flashAddrTop_;
	uint32_t flashAddrBtm_;
	uint32_t flashAddrTarget_;
public:
	UF2Installer(Printable& terr, uint32_t flashAddrTop) : terr_{terr}, completeFlag_{false},
		flashBytes_{0}, flashAddrTop_{flashAddrTop}, flashAddrBtm_{flashAddrTop}, flashAddrTarget_{0} {}
public:
	uint32_t GetFlashBytes() const { return flashBytes_; }
	uint32_t GetFlashAddrTop() const { return flashAddrTop_; }
	bool IsComplete() const { return completeFlag_; }
public:
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) override;
	virtual int32_t On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) override;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) override;
};

}
