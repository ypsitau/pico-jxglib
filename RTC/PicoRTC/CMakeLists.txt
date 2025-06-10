add_library(jxglib_PicoRTC INTERFACE)

target_include_directories(jxglib_PicoRTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(jxglib_PicoRTC INTERFACE
	hardware_rtc
	jxglib_RTC)

target_sources(jxglib_PicoRTC INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/src/DS323x.cpp)
