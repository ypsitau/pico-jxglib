add_library(jxglib_USBHost INTERFACE)

target_include_directories(jxglib_USBHost INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_USBHost INTERFACE
	jxglib_Mouse
	tinyusb_host)

target_sources(jxglib_USBHost INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/USBHost.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/HID.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/GamePad.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/MSC.cpp)

#------------------------------------------------------------------------------
add_library(jxglib_ShellCmd_USBHost_MSC INTERFACE)

target_include_directories(jxglib_ShellCmd_USBHost_MSC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_ShellCmd_USBHost_MSC INTERFACE
	jxglib_ShellCmd_Basic
	jxglib_USBHost)

target_sources(jxglib_ShellCmd_USBHost_MSC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/ShellCmd_USBHost_MSC.cpp)

#------------------------------------------------------------------------------
function(jxglib_configure_USBHost target)
	set(VARS
		CFG_TUSB_DEBUG
		CFG_TUH_ENABLED
		CFG_TUH_ENUMERATION_BUFSIZE
		CFG_TUH_HUB
		CFG_TUH_CDC
		CFG_TUH_CDC_FTDI
		CFG_TUH_CDC_CP210X
		CFG_TUH_CDC_CH34X
		CFG_TUH_HID
		CFG_TUH_MSC
		CFG_TUH_VENDOR
		CFG_TUH_MIDI
		CFG_TUH_DEVICE_MAX
		CFG_TUH_HID_EPIN_BUFSIZE
		CFG_TUH_HID_EPOUT_BUFSIZE
		CFG_TUH_CDC_LINE_CONTROL_ON_ENUM
		CFG_TUH_CDC_LINE_CODING_ON_ENUM
	)
	cmake_parse_arguments("" ""  "${VARS}" "" ${ARGN})
	jxglib_set_if_undefined(_CFG_TUSB_DEBUG						0)
	jxglib_set_if_undefined(_CFG_TUH_ENABLED					1)
	jxglib_set_if_undefined(_CFG_TUH_ENUMERATION_BUFSIZE		256)
	jxglib_set_if_undefined(_CFG_TUH_HUB						1)
	jxglib_set_if_undefined(_CFG_TUH_CDC						0)
	jxglib_set_if_undefined(_CFG_TUH_CDC_FTDI					0)
	jxglib_set_if_undefined(_CFG_TUH_CDC_CP210X					0)
	jxglib_set_if_undefined(_CFG_TUH_CDC_CH34X					0)
	jxglib_set_if_undefined(_CFG_TUH_HID						0)
	jxglib_set_if_undefined(_CFG_TUH_MSC						0)
	jxglib_set_if_undefined(_CFG_TUH_VENDOR						0)
	jxglib_set_if_undefined(_CFG_TUH_MIDI						0)
	jxglib_set_if_undefined(_CFG_TUH_DEVICE_MAX					"(3 * CFG_TUH_HUB + 1)")
	jxglib_set_if_undefined(_CFG_TUH_HID_EPIN_BUFSIZE			64)
	jxglib_set_if_undefined(_CFG_TUH_HID_EPOUT_BUFSIZE			64)
	jxglib_set_if_undefined(_CFG_TUH_CDC_LINE_CONTROL_ON_ENUM	0x03)
	jxglib_set_if_undefined(_CFG_TUH_CDC_LINE_CODING_ON_ENUM
		"{ 115200, CDC_LINE_CODING_STOP_BITS_1, CDC_LINE_CODING_PARITY_NONE, 8 }")
	configure_file(${JXGLIB_DIR}/USBHost/tusb_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/tusb_config.h)
	target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()
