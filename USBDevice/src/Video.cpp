//==============================================================================
// Video.cpp
//==============================================================================
#include "jxglib/USBDevice/Video.h"

#if CFG_TUD_VIDEO > 0

//-----------------------------------------------------------------------------
// USBDevice::Video
//-----------------------------------------------------------------------------
namespace jxglib::USBDevice {

Video::Video(Controller& deviceController) :
				Interface(deviceController, 2)
{
	//uint8_t configDesc[] = {
	//	// Interface number, string index, EP notification address and size, EP data address (out, in) and size.
	//	TUD_CDC_DESCRIPTOR(interfaceNum_, deviceController.RegisterStringDesc(str), endpNotif, bytesNotif, endpBulkOut, endpBulkIn, bytesBulk),
	//};
	//iInstance_ = deviceController.AddInterface_CDC(this);
	//RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

#endif
