#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice.h"
#include "jxglib/Flash.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// MSC_Flash
//-----------------------------------------------------------------------------
class MSC_Flash : public USBDevice::MSC {
public:
	class SyncAgent : public Tickable {
	private:
		int cntUntilSync_ ;
	public:
		SyncAgent() : Tickable(500, Tickable::Priority::Lowest), cntUntilSync_{0} {}
	public:
		void Start() { cntUntilSync_ = 2; }
	public:
		// virual functions of Tickable
		virtual void OnTick() override;
	};
public:
	static const int BlockSize = 512;
private:
	uint32_t offsetXIP_;
	uint32_t bytesXIP_;
	SyncAgent syncAgent_;
public:
	MSC_Flash(USBDevice& device, uint32_t addrXIP, uint32_t bytesXIP);
public:
	// virual functions of USBDevice::MSC
	virtual void On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) override;
	virtual bool On_msc_test_unit_ready(uint8_t lun) override;
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) override;
	virtual bool On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) override;
	virtual int32_t On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) override;
	virtual bool On_msc_is_writable(uint8_t lun) override;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) override;
	virtual int32_t On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) override;
};

MSC_Flash::MSC_Flash(USBDevice& device, uint32_t addrXIP, uint32_t bytesXIP) :
	USBDevice::MSC(device, "Flash Interface", 0x01, 0x81),
	offsetXIP_{addrXIP & 0x0fff'ffff}, bytesXIP_{bytesXIP}
{}

void MSC_Flash::On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	//::printf("On_msc_inquiry\n");
	static const char vid[] = "TinyUSB";
	static const char pid[] = "Mass Storage";
	static const char rev[] = "1.0";
	memcpy(vendor_id,	vid, strlen(vid));
	memcpy(product_id,	pid, strlen(pid));
	memcpy(product_rev,	rev, strlen(rev));
}

bool MSC_Flash::On_msc_test_unit_ready(uint8_t lun)
{
	//::printf("On_msc_test_unit_ready\n");
	return true;
}

void MSC_Flash::On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	//::printf("On_msc_capacity\n");
	*block_count = bytesXIP_ / BlockSize;
	*block_size  = BlockSize;
}

bool MSC_Flash::On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	//::printf("On_msc_start_stop\n");
	if (!load_eject) {
		// nothing to do
	} else if (start) {
		// load disk storage
	} else {
		Flash::Sync();
		// unload disk storage
	}
	return true;
}

int32_t MSC_Flash::On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	//::printf("On_msc_read10(lba=%d, bufsize=%d)\n", lba, bufsize);
	if (lba >= bytesXIP_ / BlockSize) return -1;
	//sdCard_.ReadBlock(lba, buffer, bufsize / BlockSize);
	Flash::Read(offsetXIP_ + lba * BlockSize, buffer, bufsize);
	return bufsize;
}

bool MSC_Flash::On_msc_is_writable(uint8_t lun)
{
	//::printf("On_msc_is_writable\n");
	return true;
}

int32_t MSC_Flash::On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	//::printf("On_msc_write10(lba=%d)\n", lba);
	if (lba >= bytesXIP_ / BlockSize) return -1;
	Flash::Write(offsetXIP_ + lba * BlockSize, buffer, bufsize);
	syncAgent_.Start();
	return bufsize;
}

int32_t MSC_Flash::On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	//::printf("On_msc_scsi\n");
	switch (scsi_cmd[0]) {
	default:
		// Set Sense = Invalid Command Operation
		::tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
		// negative means error -> tinyusb could stall and/or response with failed status
		return -1;
	}
	return 0;
}

void MSC_Flash::SyncAgent::OnTick()
{
	if (cntUntilSync_ > 0) {
		--cntUntilSync_;
		if (cntUntilSync_ == 0) Flash::Sync();
	}
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBDevice device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "RPi Flash", "RPi Flash Device", "3141592653");
	MSC_Flash msc(device, 0x1018'0000, 0x008'0000);
	FAT::Flash fat(0x1018'0000, 0x008'0000);
	fat.Mount();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	device.Initialize();
	msc.Initialize();
	for (;;) Tickable::Tick();
}
