//==============================================================================
// CDC.cpp
//==============================================================================
#include "jxglib/USBDevice/CDC.h"

#if CFG_TUD_CDC > 0

//-----------------------------------------------------------------------------
// USBDevice::CDC
//-----------------------------------------------------------------------------
namespace jxglib::USBDevice {

CDC::CDC(Controller& deviceController, const char* str, uint8_t endpNotif, uint8_t bytesNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, uint8_t bytesBulk, uint8_t pollingInterval) :
				Interface(deviceController, 2, pollingInterval)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP notification address and size, EP data address (out, in) and size.
		TUD_CDC_DESCRIPTOR(interfaceNum_, deviceController.RegisterStringDesc(str), endpNotif, bytesNotif, endpBulkOut, endpBulkIn, bytesBulk),
	};
	iInstance_ = deviceController.AddInterface_CDC(this);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

// Invoked when received new data
void tud_cdc_rx_cb(uint8_t iInstance)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = Controller::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_rx();
}

// Invoked when received `wanted_char`
void tud_cdc_rx_wanted_cb(uint8_t iInstance, char wanted_char)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = Controller::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_rx_wanted(wanted_char);
}  

// Invoked when a TX is complete and therefore space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t iInstance)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = Controller::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_tx_complete();
}  

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t iInstance, bool dtr, bool rts)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = Controller::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_line_state(dtr, rts);
}  

// Invoked when line coding is change via SET_LINE_CODING
void tud_cdc_line_coding_cb(uint8_t iInstance, const cdc_line_coding_t* p_line_coding)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = Controller::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_line_coding(p_line_coding);
}  

// Invoked when received send break
void tud_cdc_send_break_cb(uint8_t iInstance, uint16_t duration_ms)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = Controller::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_send_break(duration_ms);
}  

#endif
