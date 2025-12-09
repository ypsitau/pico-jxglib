//==============================================================================
// USBDevice.cpp
//==============================================================================
#include "jxglib/UTF8.h"
#include "jxglib/USBDevice.h"

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// USBDevice
//-----------------------------------------------------------------------------
Controller* Controller::Instance = nullptr;

Controller::Controller(const tusb_desc_device_t& deviceDesc, uint16_t langid,
		const char* strManufacturer, const char* strProduct, const char* strSerialNumber,
		uint8_t attr, uint16_t power_ma) : Tickable(0),
	deviceDesc_{deviceDesc}, langid_{langid}, attr_{attr}, power_ma_{power_ma}, interfaceNumCur_{0},
	offsetConfigDesc_{TUD_CONFIG_DESC_LEN}, iStringDescCur_{1},
	nInstances_CDC_{0}, nInstances_MSC_{0}, nInstances_HID_{0}
{
	for (int iInstance = 0; iInstance < nInstancesMax; iInstance++) {
		interfaceTbl_[iInstance] = nullptr;
	}
	Instance = this;
	deviceDesc_.bLength = sizeof(tusb_desc_device_t);
	deviceDesc_.bDescriptorType		= TUSB_DESC_DEVICE;
	deviceDesc_.iManufacturer		= RegisterStringDesc(strManufacturer);
	deviceDesc_.iProduct			= RegisterStringDesc(strProduct);
	deviceDesc_.iSerialNumber		= RegisterStringDesc(strSerialNumber);
	deviceDesc_.bNumConfigurations	= 1;
}

void Controller::RegisterConfigDesc(const void* configDesc, int bytes)
{
	::memcpy(configDescAccum_ + offsetConfigDesc_, configDesc, bytes);
	offsetConfigDesc_ += bytes;
}

uint8_t Controller::RegisterStringDesc(const char* str)
{
	if (!str) return 0;
	uint8_t iStringDesc = iStringDescCur_++;
	stringDescTbl_[iStringDesc] = str;
	return iStringDesc;
}

uint8_t Controller::AddInterface_CDC(CDC* pCDC)
{
	uint8_t iInstance = nInstances_CDC_++;
	if (iInstance >= CFG_TUD_CDC) {
		::panic("CFG_TUD_CDC in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pCDCTbl[iInstance] = pCDC;
	return iInstance;
}

uint8_t Controller::AddInterface_MSC(MSC* pMSC) {
	uint8_t iInstance = nInstances_MSC_++;
	if (iInstance >= CFG_TUD_MSC) {
		::panic("CFG_TUD_MSC in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pMSCTbl[iInstance] = pMSC;
	return iInstance;
}

uint8_t Controller::AddInterface_HID(HID* pHID)
{
	uint8_t iInstance = nInstances_HID_++;
	if (iInstance >= CFG_TUD_HID) {
		::panic("CFG_TUD_HID in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pHIDTbl[iInstance] = pHID;
	return iInstance;
}

void Controller::Initialize(uint8_t rhport)
{
	uint8_t config_num = 1;
	uint8_t stridx = 0;
	uint8_t configDesc[] = {
		9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(offsetConfigDesc_), interfaceNumCur_, config_num,
		stridx, static_cast<uint8_t>(TU_BIT(7) | attr_), static_cast<uint8_t>(power_ma_ / 2)
	};
	::memcpy(configDescAccum_, configDesc, sizeof(configDesc));
	::tud_init(rhport);
}

uint8_t Controller::AssignInterfaceNum(int nInterfacesToOccupy)
{
	uint8_t interfaceNum = interfaceNumCur_;
	interfaceNumCur_ += nInterfacesToOccupy;
	return interfaceNum;
}

const uint16_t* Controller::On_GET_STRING_DESCRIPTOR(uint8_t idxString, uint16_t langid)
{
	//::printf("On_GET_STRING_DESCRIPTOR(%d, %04x)\n", idxString, langid);
	int bytesDst = 0;
	if (idxString == 0) {
		stringDesc_[1] = langid_;
		bytesDst = 2;
	} else {
		UTF8::Decoder decoder;
		uint8_t* pDst = reinterpret_cast<uint8_t*>(&stringDesc_[1]);
		for (const char* pSrc = stringDescTbl_[idxString]; *pSrc; pSrc++) {
			uint32_t codeUTF32;
			if (decoder.FeedChar(*pSrc, &codeUTF32)) {
				if (bytesDst + 2 > sizeof(stringDesc_) - 2) break;
				*pDst++ = static_cast<uint8_t>(codeUTF32 & 0xff);
				*pDst++ = static_cast<uint8_t>((codeUTF32 >> 8) & 0xff);
				bytesDst += 2;
			}
		}
	}
	stringDesc_[0] = static_cast<uint16_t>((TUSB_DESC_STRING << 8) | static_cast<uint8_t>(bytesDst + 2));
	return stringDesc_;
}

//-----------------------------------------------------------------------------
// USBDevice::Interface
//-----------------------------------------------------------------------------
Interface::Interface(Controller& deviceController, int nInterfacesToOccupy) :
		deviceController_{deviceController}, iInstance_{0}
{
	interfaceNum_ = deviceController.AssignInterfaceNum(nInterfacesToOccupy);
}
	
void Interface::RegisterConfigDesc(const void* configDesc, int bytes)
{
	deviceController_.RegisterConfigDesc(configDesc, bytes);
}

}

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------
extern "C" void tud_mount_cb()
{
	jxglib::USBDevice::Controller::Instance->OnMount();
}

extern "C" void tud_umount_cb()
{
	jxglib::USBDevice::Controller::Instance->OnUmount();
}

// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
extern "C" void tud_suspend_cb(bool remote_wakeup_en)
{
	jxglib::USBDevice::Controller::Instance->OnSuspend();
}

extern "C" void tud_resume_cb()
{
	jxglib::USBDevice::Controller::Instance->OnResume();
}

// Invoked when received GET DEVICE DESCRIPTOR
extern "C" const uint8_t* tud_descriptor_device_cb()
{
	return jxglib::USBDevice::Controller::Instance->On_GET_DEVICE_DESCRIPTOR();
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
//extern "C" const uint8_t* tud_descriptor_configuration_cb(uint8_t idxConfig)
//{
//	return jxglib::USBDevice::Controller::Instance->On_GET_CONFIGURATION_DESCRIPTOR(idxConfig);
//}

// Invoked when received GET STRING DESCRIPTOR request
extern "C" const uint16_t* tud_descriptor_string_cb(uint8_t idxString, uint16_t langid)
{
	return jxglib::USBDevice::Controller::Instance->On_GET_STRING_DESCRIPTOR(idxString, langid);
}
