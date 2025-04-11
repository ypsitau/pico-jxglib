//==============================================================================
// Util
//==============================================================================
#include "jxglib/Util.h"

namespace jxglib {

void WaitMSecSinceBoot(uint32_t msec)
{
    while (::to_ms_since_boot(::get_absolute_time()) < msec) ;
}

}
