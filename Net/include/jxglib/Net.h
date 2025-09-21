//==============================================================================
// jxglib/Net.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_H
#define PICO_JXGLIB_NET_H
#include "pico/stdlib.h"
#include "jxglib/WiFi.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"

namespace jxglib::Net {

inline void lwip_begin() { ::cyw43_arch_lwip_begin(); }
inline void lwip_end() { ::cyw43_arch_lwip_end(); }

}

#endif
