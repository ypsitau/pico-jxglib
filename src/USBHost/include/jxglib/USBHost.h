//==============================================================================
// jxglib/USBHost.h
//==============================================================================
#ifndef PICO_JXGLIB_USBHOST_H
#define PICO_JXGLIB_USBHOST_H
#include <memory>
#include "pico/stdlib.h"
#include "tusb.h"
#include "jxglib/FIFOBuff.h"
#include "jxglib/Tickable.h"

namespace jxglib::USBHost {

class EventHandler;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
void Initialize(uint8_t rhport = BOARD_TUH_RHPORT, EventHandler* pEventHandler = nullptr);

//------------------------------------------------------------------------------
// USBHost::EventHandler
//------------------------------------------------------------------------------
class EventHandler {
public:
	virtual void OnMount(uint8_t devAddr) {}
	virtual void OnUmount(uint8_t devAddr) {}
};

//------------------------------------------------------------------------------
// Controller
//------------------------------------------------------------------------------
class Controller : public Tickable {
public:
	Controller();
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "USBHost::Controller"; }
	virtual void OnTick() override;
};

}

#endif
