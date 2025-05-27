//==============================================================================
// USBHost.cpp
//==============================================================================
#include "jxglib/USBHost.h"

namespace jxglib::USBHost {

static Controller Instance;
static EventHandler* pEventHandler = nullptr;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
void Initialize(uint8_t rhport, EventHandler* pEventHandler)
{
	::tuh_init(rhport);
	pEventHandler = pEventHandler;
}

//------------------------------------------------------------------------------
// USBHost::Controller
//------------------------------------------------------------------------------
Controller::Controller()
{
}

void Controller::OnTick()
{
	::tuh_task();
}

}

//------------------------------------------------------------------------------
// Callback functions
//------------------------------------------------------------------------------
extern "C" void tuh_mount_cb(uint8_t devAddr)
{
	using namespace jxglib::USBHost;
	if (pEventHandler) pEventHandler->OnMount(devAddr);
}

extern "C" void tuh_umount_cb(uint8_t devAddr)
{
	using namespace jxglib::USBHost;
	if (pEventHandler) pEventHandler->OnUmount(devAddr);
}
