add_library(jxglib_USBHost INTERFACE)

target_include_directories(jxglib_USBHost INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_USBHost INTERFACE
	jxglib_Keyboard
	jxglib_Mouse
	tinyusb_host)

target_sources(jxglib_USBHost INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/USBHost.cpp)
