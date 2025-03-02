add_library(jxglib_USBD INTERFACE)

target_include_directories(jxglib_USBD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_USBD INTERFACE
	jxglib_Common
	tinyusb_device)
#	tinyusb_host

target_sources(jxglib_USBD INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/USBD.cpp)
