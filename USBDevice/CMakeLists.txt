add_library(jxglib_USBDevice INTERFACE)

target_include_directories(jxglib_USBDevice INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_USBDevice INTERFACE
	jxglib_Common
	tinyusb_device)

target_sources(jxglib_USBDevice INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/USBDevice.cpp)
