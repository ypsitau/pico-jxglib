add_library(jxglib_USBDevice INTERFACE)

target_include_directories(jxglib_USBDevice INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_USBDevice INTERFACE
	jxglib_Common
	tinyusb_device)

target_sources(jxglib_USBDevice INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/USBDevice.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/HID.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/CDC.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/MSC.cpp)

function(jxglib_configure_USBDevice target)
	set(VARS
		CFG_TUSB_OS
		CFG_TUSB_DEBUG
		CFG_TUD_ENABLED
		CFG_TUD_ENDPOINT0_SIZE
		CFG_TUD_CDC
		CFG_TUD_HID
		CFG_TUD_MSC
		CFG_TUD_MIDI
		CFG_TUD_VENDOR
		CFG_TUD_AUDIO
		CFG_TUD_CDC_RX_BUFSIZE
		CFG_TUD_CDC_TX_BUFSIZE
		CFG_TUD_CDC_EP_BUFSIZE
		CFG_TUD_HID_EP_BUFSIZE
		CFG_TUD_MSC_EP_BUFSIZE
	)
	cmake_parse_arguments("" ""  "${VARS}" "" ${ARGN})
	jxglib_set_if_undefined(_CFG_TUSB_OS						OPT_OS_NONE)
	jxglib_set_if_undefined(_CFG_TUSB_DEBUG						0)
	jxglib_set_if_undefined(_CFG_TUD_ENABLED					1)
	jxglib_set_if_undefined(_CFG_TUD_ENDPOINT0_SIZE				64)
	jxglib_set_if_undefined(_CFG_TUD_CDC						0)
	jxglib_set_if_undefined(_CFG_TUD_HID						0)
	jxglib_set_if_undefined(_CFG_TUD_MSC						0)
	jxglib_set_if_undefined(_CFG_TUD_MIDI						0)
	jxglib_set_if_undefined(_CFG_TUD_VENDOR						0)
	jxglib_set_if_undefined(_CFG_TUD_AUDIO						0)
	jxglib_set_if_undefined(_CFG_TUD_CDC_RX_BUFSIZE				"(TUD_OPT_HIGH_SPEED? 512 : 64)")
	jxglib_set_if_undefined(_CFG_TUD_CDC_TX_BUFSIZE				"(TUD_OPT_HIGH_SPEED? 512 : 64)")
	jxglib_set_if_undefined(_CFG_TUD_CDC_EP_BUFSIZE				"(TUD_OPT_HIGH_SPEED? 512 : 64)")
	jxglib_set_if_undefined(_CFG_TUD_HID_EP_BUFSIZE				64)
	jxglib_set_if_undefined(_CFG_TUD_MSC_EP_BUFSIZE				512)
	configure_file(${JXGLIB_DIR}/USBDevice/tusb_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/tusb_config.h)
		target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

#------------------------------------------------------------------------------
add_library(jxglib_USBDevice_Serial INTERFACE)

target_link_libraries(jxglib_USBDevice_Serial INTERFACE
	jxglib_Serial
	jxglib_USBDevice)

target_sources(jxglib_USBDevice_Serial INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Serial.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_USBDevice_Flash INTERFACE)

target_link_libraries(jxglib_USBDevice_Flash INTERFACE
	jxglib_Flash
	jxglib_USBDevice)

target_sources(jxglib_USBDevice_Flash INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/Flash.cpp)
