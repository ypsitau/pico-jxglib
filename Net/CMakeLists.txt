add_library(jxglib_Net INTERFACE)

target_include_directories(jxglib_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_Net INTERFACE
	pico_cyw43_arch_lwip_poll
	#pico_cyw43_arch_lwip_threadsafe_background
	jxglib_Common)

target_sources(jxglib_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/WiFi.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/Net.cpp)

target_compile_definitions(cyw43_driver INTERFACE
	NDEBUG=1)

#------------------------------------------------------------------------------
add_library(jxglib_NetUtil INTERFACE)

target_include_directories(jxglib_NetUtil INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_NetUtil INTERFACE
	jxglib_RTC
	jxglib_Net
	jxglib_Net_TCP
	jxglib_Net_UDP)

target_sources(jxglib_NetUtil INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/NetUtil.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_Net INTERFACE)

target_include_directories(jxglib_ShellCmd_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_Net INTERFACE
	jxglib_Net
	jxglib_ShellCmd_Basic)

target_sources(jxglib_ShellCmd_Net INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_Net.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_NetUtil INTERFACE)

target_include_directories(jxglib_ShellCmd_NetUtil INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_NetUtil INTERFACE
	jxglib_NetUtil
	jxglib_ShellCmd_Net)

target_sources(jxglib_ShellCmd_NetUtil INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_NetUtil.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_LED INTERFACE)

target_include_directories(jxglib_ShellCmd_LED INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_LED INTERFACE
	jxglib_ShellCmd_Basic)

if(PICO_BOARD MATCHES ".*_w$")
	target_link_libraries(jxglib_ShellCmd_LED INTERFACE
		jxglib_Net)
endif()

target_sources(jxglib_ShellCmd_LED INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_LED.cpp)

#------------------------------------------------------------------------------
add_subdirectory(TCP)
add_subdirectory(UDP)
add_subdirectory(Telnet)

#------------------------------------------------------------------------------
function(jxglib_configure_Net TARGET)
	set(VARS
		NO_SYS
		LWIP_SOCKET
		MEM_ALIGNMENT
		MEM_SIZE
		MEMP_NUM_TCP_SEG
		MEMP_NUM_ARP_QUEUE
		PBUF_POOL_SIZE
		LWIP_ARP
		LWIP_ETHERNET
		LWIP_ICMP
		LWIP_RAW
		TCP_WND
		TCP_MSS
		TCP_SND_BUF
		TCP_SND_QUEUELEN
		LWIP_NETIF_STATUS_CALLBACK
		LWIP_NETIF_LINK_CALLBACK
		LWIP_NETIF_HOSTNAME
		LWIP_NETCONN
		MEM_STATS
		SYS_STATS
		MEMP_STATS
		LINK_STATS
		ETH_PAD_SIZE
		LWIP_CHKSUM_ALGORITHM
		LWIP_DHCP
		LWIP_IPV4
		LWIP_TCP
		LWIP_UDP
		LWIP_DNS
		LWIP_TCP_KEEPALIVE
		LWIP_NETIF_TX_SINGLE_PBUF
		DHCP_DOES_ARP_CHECK
		LWIP_DHCP_DOES_ACD_CHECK
		LWIP_DEBUG
		LWIP_STATS
		LWIP_STATS_DISPLAY
		ETHARP_DEBUG
		NETIF_DEBUG
		PBUF_DEBUG
		API_LIB_DEBUG
		API_MSG_DEBUG
		SOCKETS_DEBUG
		ICMP_DEBUG
		INET_DEBUG
		IP_DEBUG
		IP_REASS_DEBUG
		RAW_DEBUG
		MEM_DEBUG
		MEMP_DEBUG
		SYS_DEBUG
		TCP_DEBUG
		TCP_INPUT_DEBUG
		TCP_OUTPUT_DEBUG
		TCP_RTO_DEBUG
		TCP_CWND_DEBUG
		TCP_WND_DEBUG
		TCP_FR_DEBUG
		TCP_QLEN_DEBUG
		TCP_RST_DEBUG
		UDP_DEBUG
		TCPIP_DEBUG
		PPP_DEBUG
		SLIP_DEBUG
		DHCP_DEBUG
	)
	cmake_parse_arguments("" ""  "${VARS}" "" ${ARGN})
	jxglib_set_if_undefined(_NO_SYS                      1)
	jxglib_set_if_undefined(_LWIP_SOCKET                 0)
	jxglib_set_if_undefined(_MEM_ALIGNMENT               4)
	jxglib_set_if_undefined(_MEM_SIZE                    4000)
	jxglib_set_if_undefined(_MEMP_NUM_TCP_SEG            32)
	jxglib_set_if_undefined(_MEMP_NUM_ARP_QUEUE          10)
	jxglib_set_if_undefined(_PBUF_POOL_SIZE              24)
	jxglib_set_if_undefined(_LWIP_ARP                    1)
	jxglib_set_if_undefined(_LWIP_ETHERNET               1)
	jxglib_set_if_undefined(_LWIP_ICMP                   1)
	jxglib_set_if_undefined(_LWIP_RAW                    1)
	jxglib_set_if_undefined(_TCP_WND                     "(8 * TCP_MSS)")
	jxglib_set_if_undefined(_TCP_MSS                     1460)
	jxglib_set_if_undefined(_TCP_SND_BUF                 "(8 * TCP_MSS)")
	jxglib_set_if_undefined(_TCP_SND_QUEUELEN            "((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))")
	jxglib_set_if_undefined(_LWIP_NETIF_STATUS_CALLBACK  1)
	jxglib_set_if_undefined(_LWIP_NETIF_LINK_CALLBACK    1)
	jxglib_set_if_undefined(_LWIP_NETIF_HOSTNAME         1)
	jxglib_set_if_undefined(_LWIP_NETCONN                0)
	jxglib_set_if_undefined(_MEM_STATS                   0)
	jxglib_set_if_undefined(_SYS_STATS                   0)
	jxglib_set_if_undefined(_MEMP_STATS                  0)
	jxglib_set_if_undefined(_LINK_STATS                  0)
	jxglib_set_if_undefined(_ETH_PAD_SIZE                2)
	jxglib_set_if_undefined(_LWIP_CHKSUM_ALGORITHM       3)
	jxglib_set_if_undefined(_LWIP_DHCP                   1)
	jxglib_set_if_undefined(_LWIP_IPV4                   1)
	jxglib_set_if_undefined(_LWIP_TCP                    1)
	jxglib_set_if_undefined(_LWIP_UDP                    1)
	jxglib_set_if_undefined(_LWIP_DNS                    1)
	jxglib_set_if_undefined(_LWIP_TCP_KEEPALIVE          1)
	jxglib_set_if_undefined(_LWIP_NETIF_TX_SINGLE_PBUF   1)
	jxglib_set_if_undefined(_DHCP_DOES_ARP_CHECK         0)
	jxglib_set_if_undefined(_LWIP_DHCP_DOES_ACD_CHECK    0)
	jxglib_set_if_undefined(_LWIP_DEBUG                  1)
	jxglib_set_if_undefined(_LWIP_STATS                  1)
	jxglib_set_if_undefined(_LWIP_STATS_DISPLAY          1)
	jxglib_set_if_undefined(_ETHARP_DEBUG                LWIP_DBG_OFF)
	jxglib_set_if_undefined(_NETIF_DEBUG                 LWIP_DBG_OFF)
	jxglib_set_if_undefined(_PBUF_DEBUG                  LWIP_DBG_OFF)
	jxglib_set_if_undefined(_API_LIB_DEBUG               LWIP_DBG_OFF)
	jxglib_set_if_undefined(_API_MSG_DEBUG               LWIP_DBG_OFF)
	jxglib_set_if_undefined(_SOCKETS_DEBUG               LWIP_DBG_OFF)
	jxglib_set_if_undefined(_ICMP_DEBUG                  LWIP_DBG_OFF)
	jxglib_set_if_undefined(_INET_DEBUG                  LWIP_DBG_OFF)
	jxglib_set_if_undefined(_IP_DEBUG                    LWIP_DBG_OFF)
	jxglib_set_if_undefined(_IP_REASS_DEBUG              LWIP_DBG_OFF)
	jxglib_set_if_undefined(_RAW_DEBUG                   LWIP_DBG_OFF)
	jxglib_set_if_undefined(_MEM_DEBUG                   LWIP_DBG_OFF)
	jxglib_set_if_undefined(_MEMP_DEBUG                  LWIP_DBG_OFF)
	jxglib_set_if_undefined(_SYS_DEBUG                   LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_DEBUG                   LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_INPUT_DEBUG             LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_OUTPUT_DEBUG            LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_RTO_DEBUG               LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_CWND_DEBUG              LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_WND_DEBUG               LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_FR_DEBUG                LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_QLEN_DEBUG              LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCP_RST_DEBUG               LWIP_DBG_OFF)
	jxglib_set_if_undefined(_UDP_DEBUG                   LWIP_DBG_OFF)
	jxglib_set_if_undefined(_TCPIP_DEBUG                 LWIP_DBG_OFF)
	jxglib_set_if_undefined(_PPP_DEBUG                   LWIP_DBG_OFF)
	jxglib_set_if_undefined(_SLIP_DEBUG                  LWIP_DBG_OFF)
	jxglib_set_if_undefined(_DHCP_DEBUG                  LWIP_DBG_OFF)
	configure_file(${JXGLIB_DIR}/Net/lwipopts.h.in ${CMAKE_CURRENT_BINARY_DIR}/lwipopts.h)
	target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
