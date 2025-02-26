#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBD.h"
#include "jxglib/SDCard.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// MSC_SDCard
//-----------------------------------------------------------------------------
class MSC_SDCard : public USBD::MSC {
public:
	static const int BlockSize = 512;
private:
	SDCard& sdCard_;
	bool ejected_;
public:
	MSC_SDCard(USBD::Device& device, SDCard& sdCard) :
		USBD::MSC(device, "SDCard Interface", 0x01, 0x81), sdCard_{sdCard}, ejected_{false} {}
public:
	virtual void On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) override;
	virtual bool On_msc_test_unit_ready(uint8_t lun) override;
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) override;
	virtual bool On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) override;
	virtual int32_t On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) override;
	virtual bool On_msc_is_writable(uint8_t lun) override;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) override;
	virtual int32_t On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) override;
};

void MSC_SDCard::On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	::printf("On_msc_inquiry\n");
	const char vid[] = "TinyUSB";
	const char pid[] = "Mass Storage";
	const char rev[] = "1.0";
	memcpy(vendor_id,	vid, strlen(vid));
	memcpy(product_id,	pid, strlen(pid));
	memcpy(product_rev,	rev, strlen(rev));
}

bool MSC_SDCard::On_msc_test_unit_ready(uint8_t lun)
{
	::printf("On_msc_test_unit_ready\n");
	// RAM disk is ready until ejected_
	if (ejected_) {
	  // Additional Sense 3A-00 is NOT_FOUND
		::tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
		return false;
	}
	return true;
}

void MSC_SDCard::On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	::printf("On_msc_capacity\n");
	*block_count = sdCard_.GetSectors();
	*block_size  = BlockSize;
}

bool MSC_SDCard::On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	::printf("On_msc_start_stop\n");
	if (!load_eject) {
		// nothing to do
	} else if (start) {
		// load disk storage
	} else {
		// unload disk storage
		ejected_ = true;
	}
	return true;
}

int32_t MSC_SDCard::On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	//::printf("On_msc_read10(lba=%d, bufsize=%d)\n", lba, bufsize);
	if (lba >= sdCard_.GetSectors()) return -1;
	sdCard_.ReadBlock(lba, buffer, bufsize / BlockSize);
	return bufsize;
}

bool MSC_SDCard::On_msc_is_writable(uint8_t lun)
{
	::printf("On_msc_is_writable\n");
	return true;
}

int32_t MSC_SDCard::On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	::printf("On_msc_write10(lba=%d)\n", lba);
	if (lba >= sdCard_.GetSectors()) return -1;
	return bufsize;
}

int32_t MSC_SDCard::On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	::printf("On_msc_scsi\n");
	switch (scsi_cmd[0]) {
	default:
		// Set Sense = Invalid Command Operation
		::tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
		// negative means error -> tinyusb could stall and/or response with failed status
		return -1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBD::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBD::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "RPi SDCard", "RPi SDCard Device", "3141592653");
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	SDCard sdCard(spi0, 10 * 1000 * 1000, {CS: GPIO5});	// 10MHz
	MSC_SDCard msc(device, sdCard);
	sdCard.Initialize(true);
	device.Initialize();
	for (;;) {
		device.Task();
	}
	return 0;
}
